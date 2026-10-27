"""CX-30 (GEN2) blended ACC: a launch from a stop is openpilot's, since MRCC brakes back during the creep."""
from types import SimpleNamespace

from opendbc.car import Bus, gen_empty_fingerprint, structs
from opendbc.car.mazda.carcontroller import ACCEL_CMD_SCALE, MRCC_CRAWL_SPEED, CarController
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.starpilot.common.experimental_state import CEStatus

LongCtrlState = structs.CarControl.Actuators.LongControlState

# a stock standstill command that differs from openpilot's
STOCK_HOLD = 2000 - 0.5 * ACCEL_CMD_SCALE
TOGGLES = SimpleNamespace(blended_acc=True)


def blended_controller():
  CP = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False, SimpleNamespace())
  cc = CarController({Bus.pt: 'mazda_2019'}, CP)
  # CEM off, so the launch latch is the only reason to hand over
  cc.params_memory = SimpleNamespace(get_int=lambda key, default=0: CEStatus["OFF"], put_int=lambda key, value: None)
  return cc


def car_state(v_ego):
  return SimpleNamespace(
    out=SimpleNamespace(vEgo=v_ego, standstill=v_ego < 0.1, steeringTorque=0., gasPressed=False),
    acc={"ACCEL_CMD": STOCK_HOLD, "ACC_ENABLED": 1, "RESUME": 0, "HOLD": 1},
    radar_lead=True, distance_setting=3, cam_lkas=0,
    openpilot_lead_status=True, openpilot_lead_d_rel=5., openpilot_lead_v_rel=1.)


def step(cc, v_ego, state, accel):
  CC = structs.CarControl(longActive=True)
  CC.actuators.accel = accel
  CC.actuators.longControlState = state
  CS = car_state(v_ego)
  cc.update(CC.as_reader(), CS, 0, TOGGLES)
  return CS.acc["ACCEL_CMD"]


def test_long_stop_behind_a_radar_lead_stays_with_mrcc():
  cc = blended_controller()
  for _ in range(500):
    sent = step(cc, 0., LongCtrlState.stopping, -0.5)
  assert cc.blend_coeff == 0. and sent == STOCK_HOLD


def test_launch_hands_to_openpilot_and_holds_through_crawl_speed():
  cc = blended_controller()
  for _ in range(500):
    step(cc, 0., LongCtrlState.stopping, -0.5)

  # the planner leaves stopping: within 0.5 s the command is openpilot's, not MRCC's
  for _ in range(60):
    sent = step(cc, 0., LongCtrlState.starting, 1.0)
  assert cc.blend_coeff == 1. and sent == 2000 + ACCEL_CMD_SCALE

  # still crawling behind the radar lead, openpilot keeps it
  step(cc, MRCC_CRAWL_SPEED / 2, LongCtrlState.pid, 1.0)
  assert cc.launch_to_op and cc.blend_coeff == 1.

  # past crawl speed the latch lets go and ordinary blending resumes
  step(cc, MRCC_CRAWL_SPEED + 0.1, LongCtrlState.pid, 1.0)
  assert not cc.launch_to_op


def test_stopping_again_releases_the_launch():
  cc = blended_controller()
  step(cc, 0., LongCtrlState.starting, 1.0)
  assert cc.launch_to_op
  step(cc, 0.5, LongCtrlState.stopping, -0.5)
  assert not cc.launch_to_op
