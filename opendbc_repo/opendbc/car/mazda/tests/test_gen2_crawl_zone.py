"""CX-30 (GEN2) blended ACC: below crawl speed openpilot owns the car outright, stops and launches included."""
from types import SimpleNamespace

import pytest
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.common.conversions import Conversions as CV
from opendbc.car.mazda.carcontroller import ACCEL_CMD_SCALE, BLEND_JERK_MAX
from opendbc.car.mazda.carstate import OP_CRAWL_EXIT_SPEED, OP_CRAWL_SPEED, op_crawl_zone
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.common.realtime import DT_CTRL

LongCtrlState = structs.CarControl.Actuators.LongControlState

# MRCC's own standstill hold, as logged on the CX-30 (-1.0 m/s^2)
STOCK_HOLD = 2000 - 1.0 * ACCEL_CMD_SCALE


class Drive:
  """The blended controller fed a CS whose crawl zone is latched the way carstate latches it."""
  def __init__(self):
    ci = CarInterface(CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False))
    ci.CC.blended_acc = True
    self.cc = ci.CC
    self.op_crawl = False

  def step(self, v_ego, state, accel, stock=STOCK_HOLD, distance_setting=3, acc_enabled=1):
    self.op_crawl = op_crawl_zone(v_ego, self.op_crawl)
    CC = structs.CarControl(longActive=True)
    CC.actuators.accel = accel
    CC.actuators.longControlState = state
    CS = SimpleNamespace(
      out=SimpleNamespace(vEgo=v_ego, standstill=v_ego < 0.1, steeringTorque=0., gasPressed=False),
      acc={"ACCEL_CMD": stock, "ACC_ENABLED": acc_enabled, "RESUME": 0, "HOLD": 1},
      op_crawl=self.op_crawl, distance_setting=distance_setting, cam_lkas=0,
      # a lead far enough out and pulling away that neither lead trigger asks for openpilot
      openpilot_lead_status=True, openpilot_lead_d_rel=60., openpilot_lead_v_rel=1., openpilot_experimental_mode=False)
    self.actuators, _ = self.cc.update(CC.as_reader(), CS, 0)
    return CS.acc["ACCEL_CMD"]


def op(accel):
  return 2000 + accel * ACCEL_CMD_SCALE


def sent_as(accel):
  # capnp carries openpilot's accel as float32, so compare to within a hundredth of a count
  return pytest.approx(op(accel), abs=0.01)


def test_zone_is_entered_below_crawl_speed_and_left_above_the_exit_speed():
  assert op_crawl_zone(OP_CRAWL_SPEED - 0.01, False)
  assert not op_crawl_zone(OP_CRAWL_SPEED, False)
  # once in, hovering between the two speeds stays in
  assert op_crawl_zone(OP_CRAWL_SPEED + 0.5, True)
  assert not op_crawl_zone(OP_CRAWL_EXIT_SPEED, True)


def test_stop_and_standstill_are_openpilots():
  d = Drive()
  for _ in range(500):
    sent = d.step(0., LongCtrlState.stopping, -0.5)
  assert d.cc.blend_coeff == 1. and sent == sent_as(-0.5)


def test_a_launch_never_changes_hands():
  d = Drive()
  for _ in range(500):
    d.step(0., LongCtrlState.stopping, -0.5)
  for v in (0., 0.4, 1.5, OP_CRAWL_SPEED + 0.5):
    sent = d.step(v, LongCtrlState.pid, 1.0, stock=op(-0.5))
    assert d.cc.blend_coeff == 1. and sent == sent_as(1.0)


def test_an_interrupted_launch_stays_openpilots():
  # CX-30 2026-11-11: openpilot left stopping, fell back into it for 0.6 s while the car rolled at
  # 0.3-0.4 m/s, then went again. Under the old launch latch MRCC had the car in between.
  d = Drive()
  for _ in range(500):
    d.step(0., LongCtrlState.stopping, -0.5)
  for state, v, n in ((LongCtrlState.pid, 0., 20), (LongCtrlState.stopping, 0.3, 60), (LongCtrlState.pid, 0.4, 60)):
    for _ in range(n):
      d.step(v, state, 1.0 if state == LongCtrlState.pid else -0.5, stock=op(-0.5))
      assert d.cc.blend_coeff == 1.


def test_creeping_behind_a_radar_lead_is_openpilots():
  # the old gate left this to MRCC whenever its radar flag was up
  d = Drive()
  for _ in range(300):
    sent = d.step(1.0, LongCtrlState.pid, 0.3, stock=op(-0.3))
  assert d.cc.blend_coeff == 1. and sent == sent_as(0.3)


def test_above_the_exit_speed_with_nothing_asking_mrcc_takes_back_over():
  d = Drive()
  d.step(1.0, LongCtrlState.pid, 0.5)
  for _ in range(300):
    d.step(OP_CRAWL_SPEED + 0.5, LongCtrlState.pid, 0.5, stock=op(0.5))
  assert d.cc.blend_coeff == 1.
  for _ in range(400):
    d.step(OP_CRAWL_EXIT_SPEED + 0.1, LongCtrlState.pid, 0.5, stock=op(0.5))
  assert d.cc.blend_coeff == 0.


def test_entering_the_zone_hands_over_under_the_jerk_cap():
  d = Drive()
  v = 10 * CV.MPH_TO_MS
  for _ in range(300):
    d.step(v, LongCtrlState.pid, -1.0, stock=op(1.0))
  assert d.cc.blend_coeff == 0.
  coeffs = []
  for _ in range(100):
    d.step(OP_CRAWL_SPEED - 0.1, LongCtrlState.pid, -1.0, stock=op(1.0))
    coeffs.append(d.cc.blend_coeff)
  steps = [b - a for a, b in zip([0.] + coeffs, coeffs)]
  # two commands 2 m/s^2 apart: the crossfade adds no more than the cap's jerk
  assert max(steps) <= BLEND_JERK_MAX * DT_CTRL / 2.0 + 1e-9
  assert coeffs[-1] > 0.


def test_closest_stalk_setting_still_hands_over_above_the_zone():
  d = Drive()
  for _ in range(300):
    sent = d.step(20., LongCtrlState.pid, 0.2, stock=op(0.2), distance_setting=1)
  assert d.cc.blend_coeff == 1. and sent == sent_as(0.2)
