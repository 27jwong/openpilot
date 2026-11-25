from types import SimpleNamespace

import pytest
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.starpilot.car.mazda.dashboard import DistancePersonality

AGGRESSIVE, STANDARD, RELAXED = 0, 1, 2


class _Params:
  def __init__(self, safe_mode=False):
    self.safe_mode = safe_mode
    self.writes = []

  def get_bool(self, key):
    assert key == "SafeMode"
    return self.safe_mode

  def put(self, key, value):
    assert key == "LongitudinalPersonality"
    self.writes.append(value)


def _cp(identity=CAR.MAZDA_CX_30):
  return CarInterface.get_params(identity, gen_empty_fingerprint(), [], False, False, False)


def _drive(settings, *, cp=None, params=None, can_valid=None):
  cp = cp or _cp()
  params = params or _Params()
  sync = DistancePersonality()
  for i, setting in enumerate(settings):
    valid = True if can_valid is None else can_valid[i]
    sync.update(cp, SimpleNamespace(distance_setting=setting), structs.CarState(canValid=valid).as_reader(), params)
  return params.writes


def test_closest_setting_is_the_everyday_aggressive_personality():
  assert _cp().openpilotLongitudinalControl
  assert _drive([1]) == [AGGRESSIVE]


def test_each_setting_change_picks_its_personality_once():
  # main off (0) -> closest -> out to the farthest -> main off -> back on at the closest
  assert _drive([0, 1, 1, 2, 2, 3, 4, 0, 1]) == [AGGRESSIVE, STANDARD, RELAXED, RELAXED, AGGRESSIVE]


def test_staying_on_a_setting_keeps_a_personality_chosen_in_between():
  # Only changes write, so a Wheel Controls "Cycle personality" press at the closest setting holds.
  params = _Params()
  assert _drive([1] + [1] * 50, params=params) == [AGGRESSIVE]


def test_invalid_can_neither_writes_nor_swallows_the_change():
  assert _drive([1, 2, 2], can_valid=[True, False, True]) == [AGGRESSIVE, STANDARD]


def test_safe_mode_leaves_the_personality_alone():
  assert _drive([1, 2, 3], params=_Params(safe_mode=True)) == []


def test_without_openpilot_long_there_is_nothing_to_set():
  cp = _cp()
  cp.openpilotLongitudinalControl = False
  assert _drive([1, 2, 3], cp=cp) == []


@pytest.mark.parametrize('identity', (CAR.MAZDA_CX5_2022, CAR.MAZDA_CX9_2021))
def test_gen1_is_untouched(identity):
  assert _drive([1, 2, 3], cp=_cp(identity)) == []
