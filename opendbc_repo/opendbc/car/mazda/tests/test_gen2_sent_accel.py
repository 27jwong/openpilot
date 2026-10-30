"""CX-30 (GEN2) blended ACC: carOutput carries the accel the car was sent, stock MRCC share included."""
import pytest

from opendbc.car import structs
from opendbc.car.mazda.carcontroller import ACCEL_CMD_SCALE
from opendbc.car.mazda.tests.test_gen2_blended_launch import STOCK_HOLD, blended_controller, car_state

LongCtrlState = structs.CarControl.Actuators.LongControlState


def update(cc, accel, state=LongCtrlState.stopping, v_ego=0., acc_enabled=1):
  CC = structs.CarControl(longActive=True)
  CC.actuators.accel = accel
  CC.actuators.longControlState = state
  CS = car_state(v_ego)
  CS.acc["ACC_ENABLED"] = acc_enabled
  actuators, _ = cc.update(CC.as_reader(), CS, 0)
  return actuators.accel, CS.acc["ACCEL_CMD"]


def test_stock_command_is_reported_while_mrcc_drives():
  cc = blended_controller()
  for _ in range(100):
    accel, sent = update(cc, -1.0)
  assert cc.blend_coeff == 0. and sent == STOCK_HOLD
  assert accel == pytest.approx((STOCK_HOLD - 2000) / ACCEL_CMD_SCALE)


def test_openpilot_command_is_reported_once_blended_in():
  cc = blended_controller()
  for _ in range(60):
    accel, _ = update(cc, 1.0, state=LongCtrlState.pid)
  assert cc.blend_coeff == 1.
  assert accel == pytest.approx(1.0)


def test_nothing_is_claimed_before_mrcc_engages():
  # stock reads a 4000 sentinel until MRCC engages; carOutput keeps openpilot's own request
  cc = blended_controller()
  accel, _ = update(cc, -1.0, acc_enabled=0)
  assert accel == pytest.approx(-1.0)
