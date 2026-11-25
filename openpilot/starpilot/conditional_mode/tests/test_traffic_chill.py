"""Traffic Mode holds chill over CEM/CCM and the crawl zone, and lets go when it ends."""

from dataclasses import replace
from types import SimpleNamespace

import pytest

from openpilot.starpilot.conditional_mode import ConditionalModePolicy, LeadEvidence, ManualIntent, ModeChoice, ModeSettings, Reason
from openpilot.starpilot.conditional_mode.preferences import manual_for_drive
from openpilot.starpilot.conditional_mode.tests.test_policy import AUTHORITY, scene
from openpilot.starpilot.conditional_mode.traffic_chill import traffic_chill

DRIVE = 1_000_000_000
NOW = 5_000_000_000
LEAD = LeadEvidence(True, True, False, distance_m=12.0, speed_mps=11.0)
# The user's CEM: below-speed trigger with a lead (it fired at 27-29 mph on route 00000028).
SETTINGS = replace(ModeSettings(), cem_speed_mps=0.0, cem_speed_with_lead_mps=15.0)


def _verdict(requested):
  return SimpleNamespace(requested=requested, effective=None, reason='authority_unavailable')


def test_requested_traffic_mode_is_a_forced_chill_for_this_drive():
  manual = traffic_chill(_verdict(True), DRIVE, NOW)
  assert manual is not None
  assert manual.intent is ManualIntent.FORCE_CHILL
  assert (manual.drive_id, manual.observed_mono_ns) == (DRIVE, NOW)


@pytest.mark.parametrize('verdict', (None, _verdict(False), _verdict(None), SimpleNamespace()))
def test_no_override_without_a_traffic_request(verdict):
  assert traffic_chill(verdict, DRIVE, NOW) is None


def test_no_override_without_a_drive():
  assert traffic_chill(_verdict(True), 0, NOW) is None


def test_traffic_chill_takes_precedence_over_the_crawl_zone_demand():
  # plannerd: traffic_chill(...) or crawl_experimental(...)
  crawl = manual_for_drive(ManualIntent.FORCE_EXPERIMENTAL, DRIVE, NOW)
  assert (traffic_chill(_verdict(True), DRIVE, NOW) or crawl).intent is ManualIntent.FORCE_CHILL
  assert (traffic_chill(_verdict(False), DRIVE, NOW) or crawl) is crawl


def _cem_step(policy, now, traffic):
  manual = traffic_chill(_verdict(traffic), DRIVE, NOW)
  return policy.step(now, ModeChoice.CEM, manual.intent if manual is not None else ManualIntent.NONE, AUTHORITY,
                     scene(now, speed=12.0, lead=LEAD, following_lead=True, traffic_mode=traffic), SETTINGS)


def test_cem_speed_trigger_is_held_off_for_the_whole_traffic_window_and_returns_after():
  policy = ConditionalModePolicy()
  before = _cem_step(policy, 10.0, False)
  assert (before.requested_experimental, before.reason) == (True, Reason.CEM_SPEED)
  for tick in range(1, 40):
    held = _cem_step(policy, 10.0 + tick * 0.05, True)
    assert (held.requested_experimental, held.effective_longitudinal, held.reason) == (False, False, Reason.MANUAL_CHILL)
  after = _cem_step(policy, 12.0, False)
  assert (after.requested_experimental, after.reason) == (True, Reason.CEM_SPEED)


def test_ccm_traffic_veto_no_longer_forces_experimental():
  # Base CCM treats Traffic Mode as a hard veto that forces experimental; the override wins.
  policy = ConditionalModePolicy()
  base = policy.step(10.0, ModeChoice.CCM, ManualIntent.NONE, AUTHORITY, scene(10.0, traffic_mode=True))
  assert (base.requested_experimental, base.reason) == (True, Reason.CCM_VETO)
  held = policy.step(10.05, ModeChoice.CCM, traffic_chill(_verdict(True), DRIVE, NOW).intent, AUTHORITY,
                     scene(10.05, traffic_mode=True))
  assert held.requested_experimental is False
