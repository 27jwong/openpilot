"""Learned gas/brake actuator delays: the brake threshold longlagd splits on, and what the planner uses.

longlagd saves its state as JSON in LiveLongitudinalDelay (log.capnp is pinned, so it has no event of
its own); plannerd re-reads it every few seconds through LearnedDelayHost.
"""
import numpy as np

from opendbc.car.mazda.values import MazdaSafetyFlags

PARAM = "LiveLongitudinalDelay"
# bump this to invalidate saved learner state
VERSION = 1

# Bounds on any lookahead the planner will use.
MIN_LONG_DELAY = 0.05
MAX_LONG_DELAY = 1.2

# Accel request (m/s^2) below which the car has to apply its brakes rather than just close
# the throttle. It drops with speed because coast-down decel (drag, rolling resistance, engine
# braking) grows with it.
#
# Placed from 86 min of CX-30 driving (tools/tuning/analyze_longitudinal.py threshold sweep, on
# mazda-starpilot). The two-path fit was best 0.45 below the old table in 5-15 m/s and 0.35 below it
# in 15-25 m/s, which is -0.75 at both 10 and 20 m/s; above 25 m/s the sweep is flat and can't place
# it. The sweep's r2 is a broad plateau (within 0.004 for offsets of -0.65 to -0.25 from the old
# table), so this is the middle of a range, not a sharp optimum. longlagd learns nothing under 5 m/s
# (MIN_VEGO), so rather than hold -0.75 where there is no data and drag is least, the table ramps
# back to the old -0.15 at a standstill. Only the CX-30 has been measured; on any other platform
# longlagd runs in shadow mode and the planner's crossfade is a no-op (see split_delay_enabled).
BRAKE_THRESHOLD_BP = [0., 10., 20.]
BRAKE_THRESHOLD_V = [-0.15, -0.75, -0.75]

# Half-width (m/s^2) of the crossfade between the gas and brake lookaheads around the threshold.
REGIME_BLEND = 0.15

# How often plannerd re-reads the saved delays. longlagd saves every 60 s.
REFRESH_NS = 5_000_000_000


def brake_threshold(v_ego: float) -> float:
  return float(np.interp(v_ego, BRAKE_THRESHOLD_BP, BRAKE_THRESHOLD_V))


def brake_weight(a_brake: float, v_ego: float) -> float:
  """How far into the brake regime a request is: 0 is all gas, 1 is all brake. Decided from the
  accel at the brake lookahead, so braking is anticipated by the longer of the two delays."""
  return float(np.clip((brake_threshold(v_ego) + REGIME_BLEND - a_brake) / (2 * REGIME_BLEND), 0.0, 1.0))


def get_accel_from_plan_split(accel_from_delay, v_ego, gas_delay, brake_delay):
  """Sample the plan with separate gas and brake lookaheads. The brake lookahead decides which
  one applies, so braking starts as early as its longer delay needs, and the two targets are
  crossfaded across the brake threshold so the output never steps. Equal delays reduce to the
  single-delay sample."""
  a_gas, stop_gas = accel_from_delay(gas_delay)
  if brake_delay == gas_delay:
    return a_gas, stop_gas
  a_brake, stop_brake = accel_from_delay(brake_delay)
  w = brake_weight(a_brake, v_ego)
  return a_gas + w * (a_brake - a_gas), (stop_brake if w > 0.5 else stop_gas)


def split_delay_enabled(CP) -> bool:
  """Platforms whose planner uses the learned gas/brake split. Everywhere else longlagd runs in
  shadow mode and the planner keeps a single delay."""
  return CP.brand == "mazda" and bool(CP.flags & MazdaSafetyFlags.GEN2)


def saved_state_matches(CP, saved) -> bool:
  return isinstance(saved, dict) and saved.get('version') == VERSION and saved.get('carFingerprint') == CP.carFingerprint


def get_long_delays(CP, saved=None) -> tuple[float, float]:
  """(gas, brake) lookaheads for the planner. Both are the car's delay until longlagd has estimated
  both regimes on a platform that opts in; then the learned values are used."""
  gas = brake = float(CP.longitudinalActuatorDelay)
  try:
    if split_delay_enabled(CP) and saved_state_matches(CP, saved) and \
       saved['gas']['status'] == 'estimated' and saved['brake']['status'] == 'estimated':
      gas, brake = float(saved['gas']['delay']), float(saved['brake']['delay'])
  except (KeyError, TypeError, ValueError):
    pass
  if not (np.isfinite(gas) and np.isfinite(brake)):
    gas = brake = float(CP.longitudinalActuatorDelay)
  return float(np.clip(gas, MIN_LONG_DELAY, MAX_LONG_DELAY)), float(np.clip(brake, MIN_LONG_DELAY, MAX_LONG_DELAY))


class LearnedDelayHost:
  """plannerd's view of longlagd's saved delays, re-read every REFRESH_NS."""

  def __init__(self, params, CP):
    self.params, self.CP = params, CP
    self.last_refresh_ns: int | None = None
    self.delays = get_long_delays(CP)

  def refresh(self, now_ns: int) -> tuple[float, float]:
    if not split_delay_enabled(self.CP):
      return self.delays
    if self.last_refresh_ns is not None and 0 <= now_ns - self.last_refresh_ns < REFRESH_NS:
      return self.delays
    self.last_refresh_ns = now_ns
    try:
      saved = self.params.get(PARAM)
    except Exception:
      saved = None
    self.delays = get_long_delays(self.CP, saved)
    return self.delays
