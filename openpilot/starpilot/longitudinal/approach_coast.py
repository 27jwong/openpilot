"""Lift off for a slower lead early enough that coasting brings the car down to its speed (Mazda GEN2).

openpilot's MPC looks 10 s ahead, and its distance cost pulls toward a lead that is still farther than
the following distance. So while the CX-30 closed on slower traffic it kept the throttle on, and braked
once the closing showed up inside that horizon. Route 0000002a, 18:16-18:22 at 47 mph: +0.2 to +0.46
m/s^2 with the lead 85-107 m ahead and 2-3 m/s slower, then -0.4 to -1.0 from 18:24. A driver lifts off
as soon as the car ahead is slower, and lets the car coast up to it.

This is that rule as one more planner candidate. A lead d metres ahead at v_lead allows at most

  v_max = v_lead + sqrt(2 * COAST_SHARE * a_coast * (d - (t_follow * v_lead + STOP_DISTANCE)))

the speed from which coasting, at COAST_SHARE of the decel the car gets off the throttle (a_coast, just
above friction_brake_onset), arrives at the lead's speed at the MPC's own steady following distance.
Below v_max the candidate allows what cruise would (the speed headroom over base's 2 s response time).
Above it, the candidate asks to coast and never for less. It can take the throttle away, but it can't
brake: min() picks it only when the MPC, cruise and the model all want more, and braking stays theirs.

COAST_SHARE leaves the rest of the coast decel for a lead that eases off during the approach. Replayed
closed loop on routes 00000029 and 0000002a (lead-following stretches, measured plant, brake onset): 0.25-0.35
are best; 0.5 starts too late to change much.
"""
from dataclasses import dataclass
import math

import numpy as np

from opendbc.car.mazda.gen2_longitudinal import friction_brake_onset
from opendbc.car.mazda.values import MazdaSafetyFlags

COAST_MARGIN = 0.12  # m/s^2 above the brake onset, the feedback's lead on the target in a decel ramp
COAST_SHARE = 0.3
RESPONSE_TIME = 2.0  # s, base's CRUISE_ACCEL_RESPONSE_TIME
MIN_SPEED = 8.0  # m/s, stop-and-go stays base's
MIN_MODEL_PROB = 0.5
MIN_COAST_DECEL = 0.05  # m/s^2, so a steep downhill still leaves a ceiling
JERK_BP = [0., 10.0, 25., 40.]  # base's A_CRUISE_MAX_BP / J_CRUISE_VALS: cruise's own jerk limit
JERK_V = [1.6, 1.2, 0.8, 0.6]


@dataclass(frozen=True)
class ApproachLead:
  d_rel: float
  v_lead: float


def lead_from(radar_lead) -> ApproachLead | None:
  try:
    present = bool(radar_lead.present)
    d_rel, v_lead, prob = float(radar_lead.dRel), float(radar_lead.vLead), float(radar_lead.modelProb)
  except (AttributeError, TypeError, ValueError):
    return None
  if not present or not all(math.isfinite(x) for x in (d_rel, v_lead, prob)) or prob < MIN_MODEL_PROB:
    return None
  return ApproachLead(d_rel, v_lead)


def coast_accel(v_ego: float, pitch: float) -> float:
  return min(friction_brake_onset(v_ego, pitch) + COAST_MARGIN, 0.0)


def speed_ceiling(lead: ApproachLead, follow_s: float, stop_distance: float, coast_decel: float) -> float:
  v_lead = max(lead.v_lead, 0.0)
  excess = max(lead.d_rel - (follow_s * v_lead + stop_distance), 0.0)
  return v_lead + math.sqrt(2.0 * COAST_SHARE * max(coast_decel, MIN_COAST_DECEL) * excess)


class ApproachCoast:
  def __init__(self, dt: float):
    self.dt = dt
    self.reset()

  def reset(self) -> None:
    self.accel: float | None = None

  def step(self, leads: list[ApproachLead], v_ego: float, pitch: float, follow_s: float, stop_distance: float,
           accel_max: float, previous_target: float, *, blocked: bool) -> float | None:
    """The approach candidate, or None when no lead bounds the throttle.

    blocked (the driver, a reset, FCW) drops the candidate at once. Losing the lead or falling below
    MIN_SPEED hands the throttle back at cruise's jerk limit instead, so a vision lead that blinks out
    for a frame doesn't step the target up.
    """
    if blocked or not all(math.isfinite(x) for x in (v_ego, pitch, follow_s, stop_distance, accel_max, previous_target)):
      self.reset()
      return None
    if leads and v_ego >= MIN_SPEED:
      coast = coast_accel(v_ego, pitch)
      ceiling = min(speed_ceiling(lead, follow_s, stop_distance, -coast) for lead in leads)
      error = ceiling - v_ego
      target = float(np.clip(error / RESPONSE_TIME if error > 0.0 else error, coast, max(accel_max, coast)))
    elif self.accel is None or self.accel >= accel_max:
      self.reset()
      return None
    else:
      target = accel_max
    # cruise's jerk limit, from wherever the planner's output is now, so lifting off is as smooth as cruise
    jerk = float(np.interp(v_ego, JERK_BP, JERK_V)) * self.dt
    start = previous_target if self.accel is None else self.accel
    self.accel = float(np.clip(target, min(start, previous_target) - jerk, start + jerk))
    return self.accel


def owner_for(cp, dt: float) -> ApproachCoast | None:
  if cp.brand != 'mazda' or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return ApproachCoast(dt)
