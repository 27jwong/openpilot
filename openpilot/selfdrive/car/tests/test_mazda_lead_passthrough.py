"""card hands radarState's leadOne and the resolved experimental mode to Mazda's blended ACC."""

from types import SimpleNamespace

from openpilot.cereal import log
from openpilot.selfdrive.car.card import Car


def lead_state(present, d_rel=0., v_rel=0., experimental=False):
  radar = log.RadarState.new_message()
  radar.leadOne.present = present
  radar.leadOne.dRel = d_rel
  radar.leadOne.vRel = v_rel
  selfdrive_state = log.SelfdriveState.new_message()
  selfdrive_state.experimentalMode = experimental

  card = Car.__new__(Car)
  card.sm = {'radarState': radar.as_reader(), 'selfdriveState': selfdrive_state.as_reader()}
  card.CI = SimpleNamespace(CS=SimpleNamespace())
  card._update_openpilot_lead_state()
  return card.CI.CS


def test_lead_is_passed_through():
  cs = lead_state(True, d_rel=42., v_rel=-3., experimental=True)
  assert cs.openpilot_lead_status is True
  assert cs.openpilot_lead_d_rel == 42.
  assert cs.openpilot_lead_v_rel == -3.
  assert cs.openpilot_experimental_mode is True


def test_no_lead_is_passed_through():
  cs = lead_state(False)
  assert cs.openpilot_lead_status is False
  assert cs.openpilot_experimental_mode is False
