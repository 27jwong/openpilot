"""Real Ford factory/Card/SelfdriveD/Controls/native combined-owner lifecycle."""
import pytest
from opendbc.car.ford.values import CAR
from openpilot.starpilot.tests.ford_lifecycle_helpers import ford_joined as ford_joined, run_actual_producer

@pytest.fixture(autouse=True)
def combined_angle(monkeypatch, topology):
  monkeypatch.ford_angle = True
  monkeypatch.ford_topology = topology

@pytest.mark.parametrize('identity', [(CAR.FORD_MUSTANG_MACH_E_MK1,18), (CAR.FORD_ESCAPE_MK4,32), (CAR.FORD_F_150_MK14,66)])
@pytest.mark.parametrize('alpha', [False,True])
@pytest.mark.parametrize('hands_free', [False,True])
@pytest.mark.parametrize('topology', ['automatic','manual-multi'])
def test_angle_aol_brake_reverse_fault_and_hands_free(ford_joined,monkeypatch):
  assert ford_joined.host.CI.CC.bp_lat is not None
  run_actual_producer(ford_joined,monkeypatch,scenario='configuration-parity')


@pytest.mark.parametrize('identity', [(CAR.FORD_MUSTANG_MACH_E_MK1,18)])
@pytest.mark.parametrize('alpha', [False,True])
@pytest.mark.parametrize('hands_free', [True])
@pytest.mark.parametrize('topology', ['automatic'])
def test_mache_nonzero_angle_after_brake_with_hands_free(ford_joined,monkeypatch):
  monkeypatch.ford_nonzero = True
  run_actual_producer(ford_joined,monkeypatch,scenario='configuration-parity')
