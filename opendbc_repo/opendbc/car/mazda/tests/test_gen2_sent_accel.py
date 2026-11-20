"""CX-30 (GEN2) blended ACC: carOutput carries the accel the car was sent, stock MRCC share included."""
import pytest

from opendbc.car import structs
from opendbc.car.mazda.carcontroller import ACCEL_CMD_SCALE
from opendbc.car.mazda.tests.test_gen2_crawl_zone import Drive

LongCtrlState = structs.CarControl.Actuators.LongControlState

# above the crawl zone, where MRCC drives unless something asks for openpilot
CRUISING = 20.
STOCK = 2000 - 0.4 * ACCEL_CMD_SCALE


def test_stock_command_is_reported_while_mrcc_drives():
  d = Drive()
  for _ in range(100):
    sent = d.step(CRUISING, LongCtrlState.pid, -1.0, stock=STOCK)
  assert d.cc.blend_coeff == 0. and sent == STOCK
  assert d.actuators.accel == pytest.approx((STOCK - 2000) / ACCEL_CMD_SCALE)


def test_openpilot_command_is_reported_once_blended_in():
  d = Drive()
  # crawling: the zone hands over under the jerk cap, 1.4 m/s^2 apart takes ~1.2 s
  for _ in range(200):
    d.step(1.0, LongCtrlState.pid, 1.0, stock=STOCK)
  assert d.cc.blend_coeff == 1.
  assert d.actuators.accel == pytest.approx(1.0)


def test_nothing_is_claimed_before_mrcc_engages():
  # stock reads a 4000 sentinel until MRCC engages; carOutput keeps openpilot's own request
  d = Drive()
  d.step(CRUISING, LongCtrlState.pid, -1.0, stock=4000, acc_enabled=0)
  assert d.actuators.accel == pytest.approx(-1.0)
