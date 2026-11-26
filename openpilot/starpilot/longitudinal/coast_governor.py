"""Brake for a lead only when coasting would not do (Mazda GEN2).

The CX-30 decelerates without its friction brakes down to friction_brake_onset() (engine braking plus
drag: -0.26 m/s^2 at 34 mph, -0.40 at 56, -0.51 at 74 on the flat), and every request below that lights
the brake lamps. Route 00000029: 43 brake applications in 20 min engaged, 30 of them while closing on a
lead, many 15-50 s from it (17:54 at 62 mph closing 0.8 m/s braked to -0.68 for 4 s; 18:59 and 19:03
closing 0.9 m/s braked to -0.35). The fuel counter (0x420 byte 1) reads zero only under fuel cut, so
braking throws away fuel that coasting through the fuel-cut band would have kept.

When lead-following braking is what sets the planner's target (the MPC, or base's inside-gap cap after
it), and the lead is not braking itself, the target is raised to just above the friction-brake onset if
coasting there keeps the gap above FLOOR_M + FLOOR_S * v for the next HORIZON_S, predicting the lead the
way the MPC does. Otherwise the brakes are used, at the least constant decel that keeps that gap, never
more than base asked for, and held until coasting has been safe with margin for RELEASE_S, so one
approach is one clean brake application rather than a flicker.

Off below MIN_SPEED (stop-and-go stays base's), for a lead that is slow, braking or will stop within the
horizon, once already inside the floor while closing, in Traffic Mode (which is meant to lock on), and
whenever cruise, the model, a stop, a curve or the driver is what is asking to slow down.

Replayed on route 00000029's lead-following stretches against the logged leads (closed loop, measured
CX-30 plant, brake threshold and fuel model, base's inside-gap cap included): brake time 142.7 -> 114.3 s,
applications 28 -> 25, minimum gap 6.6 -> 6.5 m and headway 0.92 -> 0.90 s, accel RMS -6%, fuel -0.9%.
"""
from dataclasses import dataclass
import math

import numpy as np

from opendbc.car.mazda.gen2_longitudinal import friction_brake_onset
from opendbc.car.mazda.values import MazdaSafetyFlags

COAST_MARGIN = 0.12  # m/s^2 above the brake onset: feedback leads the target by up to ~0.1 in a decel ramp
BRAKE_MARGIN = 0.05  # m/s^2 below the onset once braking, so an application is a clean one
FLOOR_M = 5.0
FLOOR_S = 0.6
HORIZON_S = 6.0
RELEASE_SCALE = 1.15
RELEASE_S = 1.0
MIN_SPEED = 8.0  # m/s
MIN_LEAD_SPEED = 6.0  # m/s
LEAD_BRAKING = -0.3  # m/s^2 of lead accel at which it is the lead braking, and so are we
STOPPING_LEAD = 3.0  # m/s the lead is predicted to fall below within the horizon
MIN_MODEL_PROB = 0.9
ACCEL_MIN = -3.5

_TS = np.arange(0.0, HORIZON_S + 1e-6, 0.1)
_DT = np.r_[0.0, np.diff(_TS)]


@dataclass(frozen=True)
class CoastLead:
  present: bool
  d_rel: float
  v_lead: float
  a_lead: float
  a_lead_tau: float
  model_prob: float


def lead_from(radar_lead) -> CoastLead | None:
  try:
    lead = CoastLead(bool(radar_lead.present), float(radar_lead.dRel), float(radar_lead.vLead), float(radar_lead.aLeadK),
                     float(radar_lead.aLeadTau), float(radar_lead.modelProb))
  except (AttributeError, TypeError, ValueError):
    return None
  values = (lead.d_rel, lead.v_lead, lead.a_lead, lead.a_lead_tau, lead.model_prob)
  return lead if lead.present and all(math.isfinite(x) for x in values) else None


def floor_m(speed):
  return FLOOR_M + FLOOR_S * speed


class CoastGovernor:
  def __init__(self):
    self.reset()

  def reset(self) -> None:
    self.braking = False
    self.safe_since: float | None = None

  def step(self, target: float, others: float, v_ego: float, lead: CoastLead | None, pitch: float, now_s: float,
           *, blocked: bool) -> float:
    """target: the planner's lead-bounded output; others: the lowest of the non-lead candidates."""
    if (blocked or lead is None or math.isnan(others) or not all(math.isfinite(x) for x in (target, v_ego, pitch, now_s)) or
        target >= others or v_ego < MIN_SPEED or lead.v_lead < MIN_LEAD_SPEED or lead.a_lead < LEAD_BRAKING or
        lead.model_prob < MIN_MODEL_PROB or lead.d_rel <= 0.0):
      self.reset()
      return target
    onset = friction_brake_onset(v_ego, pitch)
    coast = onset + COAST_MARGIN
    a_lead = lead.a_lead * np.exp(-max(lead.a_lead_tau, 0.0) * _TS ** 2 / 2)
    v_lead = np.clip(lead.v_lead + np.cumsum(_DT * a_lead), 0.0, None)
    x_lead = lead.d_rel + np.cumsum(_DT * v_lead)
    if v_lead.min() < STOPPING_LEAD or (lead.d_rel < floor_m(v_ego) and lead.v_lead < v_ego):
      self.reset()
      return target

    def margin(accel, scale=1.0):
      v_e = np.clip(v_ego + accel * _TS, 0.0, None)
      return float(np.min(x_lead - np.cumsum(_DT * v_e) - scale * floor_m(v_e)))

    if not self.braking:
      if target >= coast:
        return target
      if margin(coast) >= 0.0:
        return min(coast, others)
      self.braking, self.safe_since = True, None
    if target >= coast:
      self.reset()
      return target
    if margin(coast, RELEASE_SCALE) >= 0.0:
      self.safe_since = now_s if self.safe_since is None else self.safe_since
      if now_s - self.safe_since >= RELEASE_S:
        self.reset()
        return min(coast, others)
    else:
      self.safe_since = None
    lo, hi = ACCEL_MIN, coast
    if margin(lo) < 0.0:
      return target
    for _ in range(18):
      mid = 0.5 * (lo + hi)
      if margin(mid) >= 0.0:
        lo = mid
      else:
        hi = mid
    return min(max(target, lo), onset - BRAKE_MARGIN, others)


def owner_for(cp) -> CoastGovernor | None:
  if cp.brand != 'mazda' or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return CoastGovernor()
