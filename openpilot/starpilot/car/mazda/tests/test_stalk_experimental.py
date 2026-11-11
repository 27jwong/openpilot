from types import SimpleNamespace

import pytest
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.starpilot.car.mazda.dashboard import MAX_AGE_NS, compose, stalk_experimental
from openpilot.starpilot.conditional_mode.policy import ManualIntent

DRIVE = 1_000_000_000
STAMP = 5_000_000_000


def _params(identity):
  return CarInterface.get_params(identity, gen_empty_fingerprint(), [], False, False, False)


def _state(*, available=True, can_valid=True):
  cs = structs.CarState(canValid=can_valid)
  cs.cruiseState.available = available
  return cs.as_reader()


def _flag(cp, setting, **state):
  message = compose(cp, SimpleNamespace(distance_setting=setting), _state(**state), None, car_state_stamp=STAMP)
  return None if message is None else message.starpilotCarState.forceExperimental


@pytest.mark.parametrize('setting, expected', ((1, True), (2, False), (3, False), (4, False), (0, False)))
def test_gen2_closest_setting_demands_experimental(setting, expected):
  assert _flag(_params(CAR.MAZDA_CX_30), setting) is expected


def test_gen2_demand_needs_valid_can_and_main_on():
  cp = _params(CAR.MAZDA_CX_30)
  assert _flag(cp, 1, can_valid=False) is False
  assert _flag(cp, 1, available=False) is False


def test_gen1_has_no_stalk_demand(monkeypatch):
  # GEN1 has a real distance button; its setting must not hijack conditional mode.
  from openpilot.starpilot.car.mazda import dashboard
  monkeypatch.setattr(dashboard, 'write_stop_sign', lambda parsed, state, message, **_: dashboard._companion(state, message, STAMP))
  message = compose(_params(CAR.MAZDA_CX5_2022), SimpleNamespace(distance_setting=1), _state(), None, car_state_stamp=STAMP)
  assert message is None or not message.starpilotCarState.forceExperimental


class _SM:
  def __init__(self, forced, *, stamp=STAMP, valid=True):
    self.logMonoTime = {'starpilotCarState': stamp}
    self.valid = {'starpilotCarState': valid}
    self._msg = SimpleNamespace(forceExperimental=forced)

  def __getitem__(self, key):
    return self._msg


def test_held_demand_becomes_this_drives_forced_experimental():
  manual = stalk_experimental(_SM(True), DRIVE, STAMP + 10_000_000)
  assert manual is not None
  assert manual.intent is ManualIntent.FORCE_EXPERIMENTAL
  assert (manual.drive_id, manual.observed_mono_ns) == (DRIVE, STAMP)


@pytest.mark.parametrize('sm, now', (
  (_SM(False), STAMP),                                    # stalk left the closest setting
  (_SM(True, valid=False), STAMP),                        # invalid companion
  (_SM(True), STAMP + MAX_AGE_NS + 1),                    # stale companion
  (_SM(True, stamp=DRIVE - 1), STAMP),                    # from before this drive
  (_SM(True), STAMP - 1),                                 # from the future
))
def test_no_override_without_a_fresh_held_demand(sm, now):
  assert stalk_experimental(sm, DRIVE, now) is None


def test_missing_service_is_no_override():
  sm = _SM(True)
  sm.logMonoTime = {}
  assert stalk_experimental(sm, DRIVE, STAMP) is None
