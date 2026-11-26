from types import SimpleNamespace
import math

import pytest

from opendbc.car.mazda.gen2_longitudinal import friction_brake_onset
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.starpilot.longitudinal.approach_coast import (COAST_MARGIN, COAST_SHARE, ApproachCoast, ApproachLead, coast_accel,
                                                             lead_from, owner_for, speed_ceiling)

DT = 0.05
FOLLOW = 0.9
STOP = 6.0


def step(ac, leads, v, prev, pitch=0.0, blocked=False, accel_max=2.0):
  return ac.step(leads, v, pitch, FOLLOW, STOP, accel_max, prev, blocked=blocked)


def settle(ac, leads, v, prev=0.0, n=200):
  out = prev
  for _ in range(n):
    out = step(ac, leads, v, out)
  return out


def test_brake_onset_matches_the_route_29_fit():
  for speed, onset in ((8.0, -0.15), (15.0, -0.26), (25.0, -0.40), (33.0, -0.51)):
    assert friction_brake_onset(speed, 0.0) == pytest.approx(onset, abs=0.01)
  # uphill gives more room before the brakes, downhill less
  assert friction_brake_onset(25.0, 0.03) < friction_brake_onset(25.0, 0.0) < friction_brake_onset(25.0, -0.03)


def test_owner_is_mazda_gen2_long_only():
  def cp(brand='mazda', long=True, flags=MazdaSafetyFlags.GEN2.value):
    return SimpleNamespace(brand=brand, openpilotLongitudinalControl=long, flags=flags)
  assert isinstance(owner_for(cp(), DT), ApproachCoast)
  assert owner_for(cp(long=False), DT) is None and owner_for(cp(flags=MazdaSafetyFlags.GEN1.value), DT) is None
  assert owner_for(cp(brand='toyota'), DT) is None


def test_ceiling_is_the_speed_coasting_brings_down_to_the_lead_at_the_following_distance():
  lead = ApproachLead(80.0, 18.0)
  decel = 0.33
  v_max = speed_ceiling(lead, FOLLOW, STOP, decel)
  # coasting at the planned share from v_max closes exactly the excess over the steady gap
  excess = 80.0 - (FOLLOW * 18.0 + STOP)
  assert (v_max - 18.0) ** 2 / (2 * COAST_SHARE * decel) == pytest.approx(excess)
  # at or inside the steady gap the ceiling is the lead's own speed
  assert speed_ceiling(ApproachLead(FOLLOW * 18.0 + STOP, 18.0), FOLLOW, STOP, decel) == pytest.approx(18.0)
  assert speed_ceiling(ApproachLead(5.0, 18.0), FOLLOW, STOP, decel) == pytest.approx(18.0)


def test_route_0000002a_18_19_lifts_off_instead_of_gassing_toward_the_slowing_lead():
  # 18:24: 47 mph, lead 63 m ahead at 35 mph, base asked +0.0 to cruise and then braked to -1.0
  ac = ApproachCoast(DT)
  out = settle(ac, [ApproachLead(63.0, 15.6)], 21.1, prev=0.05)
  assert out == pytest.approx(coast_accel(21.1, 0.0))
  assert out > friction_brake_onset(21.1, 0.0)  # coasting, not braking


def test_a_far_or_faster_lead_leaves_cruise_alone():
  ac = ApproachCoast(DT)
  assert settle(ac, [ApproachLead(150.0, 28.0)], 29.5) > 0.5  # 150 m, 1.5 m/s slower: cruise may still accelerate
  ac = ApproachCoast(DT)
  assert settle(ac, [ApproachLead(30.0, 25.0)], 22.0) > 0.5  # pulling away


def test_never_asks_for_less_than_coasting_and_lifts_off_at_cruise_jerk():
  ac = ApproachCoast(DT)
  first = step(ac, [ApproachLead(25.0, 10.0)], 25.0, 0.4)  # well over the ceiling
  assert first == pytest.approx(0.4 - 0.8 * DT)  # cruise's jerk limit at 25 m/s
  out = settle(ac, [ApproachLead(25.0, 10.0)], 25.0, prev=first)
  assert out == pytest.approx(coast_accel(25.0, 0.0)) and out >= friction_brake_onset(25.0, 0.0) + COAST_MARGIN - 1e-9


def test_the_slowest_ceiling_of_two_leads_wins():
  near_slow, far_fast = ApproachLead(40.0, 12.0), ApproachLead(90.0, 25.0)
  a = settle(ApproachCoast(DT), [near_slow, far_fast], 20.0)
  b = settle(ApproachCoast(DT), [near_slow], 20.0)
  assert a == pytest.approx(b)


def test_a_lead_that_blinks_out_hands_the_throttle_back_at_cruise_jerk():
  ac = ApproachCoast(DT)
  coast = settle(ac, [ApproachLead(25.0, 10.0)], 25.0, prev=0.0)
  out = step(ac, [], 25.0, coast)  # one frame without the lead
  assert out == pytest.approx(coast + 0.8 * DT)
  assert step(ac, [ApproachLead(25.0, 10.0)], 25.0, out) == pytest.approx(coast)  # back, no jump either way
  # gone for good: rises to accel_max at cruise jerk, then the candidate drops out
  outs = []
  for _ in range(200):
    outs.append(step(ac, [], 25.0, coast, accel_max=0.5))
    if outs[-1] is None:
      break
  assert outs[-1] is None and all(b - a <= 0.8 * DT + 1e-9 for a, b in zip(outs[:-2], outs[1:-1], strict=True))
  # slowing below MIN_SPEED releases the same way
  ac = ApproachCoast(DT)
  coast = settle(ac, [ApproachLead(25.0, 5.0)], 9.0, prev=0.0)
  assert step(ac, [ApproachLead(25.0, 5.0)], 7.9, coast) > coast


@pytest.mark.parametrize('kwargs', (
  {'v': 6.0},  # stop-and-go stays base's
  {'leads': []},
  {'blocked': True},
  {'v': math.nan},
))
def test_hands_back_to_base(kwargs):
  ac = ApproachCoast(DT)
  args = {'leads': [ApproachLead(30.0, 10.0)], 'v': 20.0, 'blocked': False} | kwargs
  assert step(ac, args['leads'], args['v'], 0.0, blocked=args['blocked']) is None
  assert ac.accel is None


def test_lead_from_rejects_absent_unsure_and_broken_leads():
  def lead(**kw):
    return SimpleNamespace(**({'present': True, 'dRel': 40.0, 'vLead': 20.0, 'modelProb': 0.9} | kw))
  assert lead_from(lead()) == ApproachLead(40.0, 20.0)
  assert lead_from(lead(present=False)) is None
  assert lead_from(lead(modelProb=0.3)) is None
  assert lead_from(lead(dRel=math.nan)) is None
  assert lead_from(SimpleNamespace()) is None
