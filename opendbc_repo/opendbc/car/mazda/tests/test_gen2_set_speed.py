"""CX-30 (GEN2) set speed: openpilot cruises in the car's own speed frame, so the dash reads the set speed."""
import pytest

from opendbc.can import CANPacker
from opendbc.car import gen_empty_fingerprint
from opendbc.car.common.conversions import Conversions as CV
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR


def state(set_speed, imperial=True, car=CAR.MAZDA_CX_30):
  ci = CarInterface(CarInterface.get_params(car, gen_empty_fingerprint(), [], False, False, False))
  packer = CANPacker('mazda_2019')
  frames = [packer.make_can_msg('CRUZE_STATE', 0, {'CRZ_STATE': 3, 'CRZ_SPEED': set_speed}),
            packer.make_can_msg('SYSTEM_SETTINGS', 0, {'IMPERIAL_UNIT': int(imperial)})]
  out = None
  for tick in range(1, 4):
    out = ci.update([(tick * 10_000_000, frames)])
  return ci.CP, out


@pytest.mark.parametrize('set_speed', (25, 45, 75))
def test_target_is_the_set_speed_in_the_dash_frame(set_speed):
  cp, cs = state(set_speed)
  # the driver's number is what the UI shows
  assert cs.cruiseState.speedCluster == pytest.approx(set_speed * CV.MPH_TO_MS)
  # the planner target is in vEgo's (true) frame: the wheel speed the dash shows reaches the set speed
  assert cs.cruiseState.speed == pytest.approx(set_speed * CV.MPH_TO_MS * cp.wheelSpeedFactor)
  assert cs.cruiseState.speed / cp.wheelSpeedFactor == pytest.approx(cs.cruiseState.speedCluster)


def test_metric_set_speed_scales_the_same_way():
  cp, cs = state(100, imperial=False)
  assert cs.cruiseState.speedCluster == pytest.approx(100 * CV.KPH_TO_MS)
  assert cs.cruiseState.speed == pytest.approx(100 * CV.KPH_TO_MS * cp.wheelSpeedFactor)


def test_unset_speed_stays_zero():
  # cruise.py reads speed == 0 as unset; the scaling must not turn it into anything else
  _, cs = state(0)
  assert cs.cruiseState.speed == 0.0 and cs.cruiseState.speedCluster == 0.0


def test_the_factor_is_the_cx30_tires():
  cp, _ = state(45)
  assert cp.wheelSpeedFactor == pytest.approx(2285 / 2179)
