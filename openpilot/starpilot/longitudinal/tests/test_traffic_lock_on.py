from types import SimpleNamespace

import pytest

from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.starpilot.longitudinal.profile_runtime import ProfileTuning, TrafficSettings, resolve
from openpilot.starpilot.longitudinal.traffic_lock_on import LOCK_ON_FOLLOW, traffic_lock_on

# the CX-30's saved Traffic/Relaxed values on route 00000029
SETTINGS = TrafficSettings(True, 'valid', None, False, (0.65, 2.0),
                           ((1.20, 1.80), (1.10, 1.50), (1.20, 1.80), (1.10, 1.50), (0.75, 1.00)))
CX30 = SimpleNamespace(brand='mazda', openpilotLongitudinalControl=True, flags=MazdaSafetyFlags.GEN2.value,
                       transmissionType=None, carFingerprint='MAZDA_CX_30')


def base(speed, settings=SETTINGS):
  return resolve(settings, 0, speed, SimpleNamespace(transmissionType=None, carFingerprint='MAZDA_CX_30'), traffic_mode=True)


@pytest.mark.parametrize('speed', (0.0, 5.0, 12.5, 20.0, 25.0, 35.0))
def test_follow_rises_to_the_lock_on_follow_not_relaxed(speed):
  tuning = traffic_lock_on(CX30, base(speed), SETTINGS, speed)
  ratio = min(speed / 25.0, 1.0)
  assert tuning.follow_seconds == pytest.approx(0.65 + (LOCK_ON_FOLLOW - 0.65) * ratio)
  assert tuning.follow_seconds <= base(speed).follow_seconds + 1e-9


def test_route_29_traffic_speeds_follow_tighter_than_aggressive():
  # 28 mph: base Traffic follows 1.33 s, Aggressive 1.0 s, lock-on 0.83 s
  speed = 28 * 0.44704
  assert base(speed).follow_seconds == pytest.approx(1.33, abs=0.01)
  assert traffic_lock_on(CX30, base(speed), SETTINGS, speed).follow_seconds == pytest.approx(0.83, abs=0.01)


@pytest.mark.parametrize('speed', (0.0, 12.5, 30.0))
def test_traffic_jerk_holds_at_every_speed(speed):
  tuning = traffic_lock_on(CX30, base(speed), SETTINGS, speed)
  assert (tuning.acceleration_jerk, tuning.deceleration_jerk, tuning.speed_jerk, tuning.speed_decrease_jerk,
          tuning.danger_jerk) == pytest.approx((1.20, 1.10, 1.20, 1.10, 0.75))


def test_document_follow_curve_is_kept():
  custom = ProfileTuning('traffic', 2.25, 1.5, 1.3, 1.5, 1.3, 0.9, 0.6, 0.42)
  assert traffic_lock_on(CX30, custom, SETTINGS, 12.5).follow_seconds == 2.25


def test_traffic_follow_above_the_lock_on_value_is_not_raised():
  settings = TrafficSettings(True, 'valid', None, False, (1.5, 2.0), SETTINGS.jerk)
  tuning = traffic_lock_on(CX30, base(12.5, settings), settings, 12.5)
  assert tuning.follow_seconds == pytest.approx(1.5)


def test_other_personalities_and_cars_pass_through():
  aggressive = ProfileTuning('aggressive', 1.0, 1.4, 1.3, 1.4, 1.3, 0.5)
  assert traffic_lock_on(CX30, aggressive, SETTINGS, 12.5) is aggressive
  assert traffic_lock_on(CX30, None, SETTINGS, 12.5) is None
  tuning = base(12.5)
  for cp in (SimpleNamespace(brand='toyota', openpilotLongitudinalControl=True, flags=MazdaSafetyFlags.GEN2.value),
             SimpleNamespace(brand='mazda', openpilotLongitudinalControl=True, flags=MazdaSafetyFlags.GEN1.value),
             SimpleNamespace(brand='mazda', openpilotLongitudinalControl=False, flags=MazdaSafetyFlags.GEN2.value)):
    assert traffic_lock_on(cp, tuning, SETTINGS, 12.5) is tuning
  invalid = TrafficSettings(False, 'invalid_master', None, False, (0.75, 1.6), ())
  assert traffic_lock_on(CX30, tuning, invalid, 12.5) is tuning
