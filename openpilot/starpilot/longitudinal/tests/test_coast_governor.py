from types import SimpleNamespace

import numpy as np
import pytest

from opendbc.car.mazda.gen2_longitudinal import friction_brake_onset
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.starpilot.longitudinal.coast_governor import (BRAKE_MARGIN, COAST_MARGIN, RELEASE_S, CoastGovernor, CoastLead,
                                                              floor_m, lead_from, owner_for)

INF = float('inf')


def lead(d, v, a=0.0, tau=1.5, prob=1.0):
  return CoastLead(True, d, v, a, tau, prob)


def test_brake_onset_matches_the_route_29_fit():
  for speed, onset in ((8.0, -0.15), (15.0, -0.26), (25.0, -0.40), (33.0, -0.51)):
    assert friction_brake_onset(speed, 0.0) == pytest.approx(onset, abs=0.01)
  # uphill gives more room before the brakes, downhill less
  assert friction_brake_onset(25.0, 0.03) < friction_brake_onset(25.0, 0.0) < friction_brake_onset(25.0, -0.03)


def test_owner_is_mazda_gen2_long_only():
  def cp(brand='mazda', long=True, flags=MazdaSafetyFlags.GEN2.value):
    return SimpleNamespace(brand=brand, openpilotLongitudinalControl=long, flags=flags)
  assert isinstance(owner_for(cp()), CoastGovernor)
  assert owner_for(cp(long=False)) is None and owner_for(cp(flags=MazdaSafetyFlags.GEN1.value)) is None
  assert owner_for(cp(brand='toyota')) is None


def test_closing_slowly_on_a_far_lead_coasts_instead_of_braking():
  # route 00000029 17:54-ish: 62 mph, a lead 0.8 m/s slower 32 m ahead; base asked -0.68
  gov = CoastGovernor()
  out = gov.step(-0.68, INF, 27.8, lead(45.0, 27.0), 0.0, 0.0, blocked=False)
  assert out == pytest.approx(friction_brake_onset(27.8, 0.0) + COAST_MARGIN)
  assert out > friction_brake_onset(27.8, 0.0)


def test_never_brakes_harder_than_base_and_never_above_the_other_candidates():
  gov = CoastGovernor()
  assert gov.step(-0.1, INF, 25.0, lead(60.0, 24.0), 0.0, 0.0, blocked=False) == -0.1  # already coasting
  assert gov.step(-0.68, -0.5, 27.8, lead(45.0, 27.0), 0.0, 0.0, blocked=False) == -0.5  # cruise's -0.5 still applies
  # coasting would land inside the floor: brake, but only what keeps the floor, and cleanly below the onset
  gov = CoastGovernor()
  out = gov.step(-1.2, INF, 25.0, lead(30.0, 20.0), 0.0, 0.0, blocked=False)
  assert -1.2 <= out <= friction_brake_onset(25.0, 0.0) - BRAKE_MARGIN + 1e-9


@pytest.mark.parametrize('kwargs', (
  dict(v_ego=6.0),                          # stop-and-go stays base's
  dict(the_lead=lead(40.0, 5.0)),            # slow lead
  dict(the_lead=lead(40.0, 20.0, a=-0.5)),   # the lead is braking
  dict(the_lead=lead(15.0, 20.0)),          # already inside the floor and closing
  dict(the_lead=lead(40.0, 20.0, prob=0.6)),  # an unsure vision lead
  dict(the_lead=None),
  dict(blocked=True),
))
def test_hands_back_to_base(kwargs):
  v_ego = kwargs.get('v_ego', 22.0)
  the_lead = kwargs.get('the_lead', lead(40.0, 20.0))
  gov = CoastGovernor()
  assert gov.step(-0.9, INF, v_ego, the_lead, 0.0, 0.0, blocked=kwargs.get('blocked', False)) == -0.9
  assert not gov.braking
  # and a lead the car is already too close to while closing stays base's whatever the governor was doing
  gov.braking = True
  assert gov.step(-0.9, INF, 22.0, lead(15.0, 20.0), 0.0, 1.0, blocked=False) == -0.9


def approach(gov, v0=25.0, vl=21.0, d0=70.0, base_decel=-0.8, seconds=25.0, dt=0.05):
  """A simple closing approach: ego obeys the governed target after 0.3 s, the lead holds its speed."""
  v, d, sent, brakes, hist = v0, d0, [], [], [0.0] * 6
  for k in range(int(seconds / dt)):
    want = base_decel if v > vl + 0.05 else 0.0
    out = gov.step(want, INF, v, lead(d, vl), 0.0, k * dt, blocked=False)
    hist.append(out); a = hist[-7]
    v = max(0.0, v + a * dt); d += (vl - v) * dt
    sent.append(out); brakes.append(out < friction_brake_onset(v, 0.0))
  return np.array(sent), np.array(brakes), d


def test_one_approach_is_one_clean_brake_application():
  # 4 m/s faster than the lead from 70 m: coasting alone would not hold the floor, so it brakes once
  sent, brakes, d = approach(CoastGovernor())
  onsets = int(np.sum(brakes[1:] & ~brakes[:-1]))
  assert onsets <= 1
  assert d >= floor_m(21.0) - 0.5
  # and it brakes less than base would have
  _, base_brakes, _ = approach(SimpleNamespace(step=lambda t, *a, **k: t))
  assert brakes.sum() < base_brakes.sum()


def test_a_tight_approach_brakes_once_and_holds_the_floor():
  # 7 m/s faster from 45 m: coasting cannot do it, so it brakes - once, and less than base
  sent, brakes, d = approach(CoastGovernor(), v0=25.0, vl=18.0, d0=45.0, base_decel=-1.5)
  assert brakes.any()
  assert int(np.sum(brakes[1:] & ~brakes[:-1])) + int(brakes[0]) == 1
  assert d >= floor_m(18.0) - 1.0
  base_sent, _, _ = approach(SimpleNamespace(step=lambda t, *a, **k: t), v0=25.0, vl=18.0, d0=45.0, base_decel=-1.5)
  # gentler and longer: less of the decel goes through the friction brakes
  friction = lambda s: float(np.sum(np.clip(friction_brake_onset(20.0, 0.0) - s, 0.0, None)))
  assert friction(sent) < friction(base_sent) and sent.min() >= base_sent.min()


def test_a_coastable_approach_never_brakes():
  # 1 m/s faster from 60 m at 60 mph: coasting closes the speed gap long before the floor
  sent, brakes, d = approach(CoastGovernor(), v0=27.0, vl=26.0, d0=60.0, base_decel=-0.6)
  assert not brakes.any()
  assert d > floor_m(26.0)


def test_braking_releases_only_after_coasting_has_been_safe_for_a_while():
  gov = CoastGovernor()
  close = lead(32.0, 20.0)
  assert gov.step(-1.0, INF, 25.0, close, 0.0, 0.0, blocked=False) < friction_brake_onset(25.0, 0.0)
  assert gov.braking
  far = lead(80.0, 24.5)
  assert gov.step(-0.6, INF, 25.0, far, 0.0, 0.05, blocked=False) < friction_brake_onset(25.0, 0.0)
  assert gov.braking
  out = gov.step(-0.6, INF, 25.0, far, 0.0, 0.05 + RELEASE_S + 0.01, blocked=False)
  assert out == pytest.approx(friction_brake_onset(25.0, 0.0) + COAST_MARGIN) and not gov.braking


def test_lead_from_reads_radard_leads():
  raw = SimpleNamespace(present=True, dRel=30.0, vLead=20.0, aLeadK=0.1, aLeadTau=1.5, modelProb=0.97)
  assert lead_from(raw) == CoastLead(True, 30.0, 20.0, 0.1, 1.5, 0.97)
  assert lead_from(SimpleNamespace(present=False, dRel=0.0, vLead=0.0, aLeadK=0.0, aLeadTau=0.0, modelProb=0.0)) is None
  assert lead_from(SimpleNamespace(present=True, dRel=float('nan'), vLead=0.0, aLeadK=0.0, aLeadTau=0.0, modelProb=1.0)) is None
