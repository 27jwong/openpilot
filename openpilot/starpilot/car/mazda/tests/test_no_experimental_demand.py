"""GEN2 has no experimental-mode demand of its own: CEM and the driver's saved choice decide, the crawl zone included."""
from types import SimpleNamespace

import pytest
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.starpilot.car.mazda.dashboard import compose

STAMP = 5_000_000_000


def _state():
  cs = structs.CarState(canValid=True)
  cs.cruiseState.available = True
  return cs.as_reader()


@pytest.mark.parametrize('op_crawl', (False, True))
@pytest.mark.parametrize('setting', (0, 1, 2, 3, 4))
def test_gen2_publishes_no_companion_of_its_own(setting, op_crawl):
  cp = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False)
  parsed = SimpleNamespace(distance_setting=setting, op_crawl=op_crawl)
  assert compose(cp, parsed, _state(), None, car_state_stamp=STAMP) is None
