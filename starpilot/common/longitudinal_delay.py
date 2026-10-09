import math

import numpy as np

from cereal import custom
from opendbc.car.mazda.values import MazdaSafetyFlags

LongDelayStatus = custom.StarPilotLongitudinalDelay.Status

# Bounds on any lookahead the planner will use, learned or set by hand.
MIN_LONG_DELAY = 0.05
MAX_LONG_DELAY = 1.2

# Accel request (m/s^2) below which the car has to apply its brakes rather than just close
# the throttle. It drops with speed because coast-down decel (drag, rolling resistance, engine
# braking) grows with it. Placeholder until tools/tuning/analyze_longitudinal.py has been run
# on routes for the platform.
BRAKE_THRESHOLD_BP = [0., 10., 25.]
BRAKE_THRESHOLD_V = [-0.15, -0.30, -0.45]

# Half-width (m/s^2) of the crossfade between the gas and brake lookaheads around the threshold.
REGIME_BLEND = 0.15


def brake_threshold(v_ego: float) -> float:
  return float(np.interp(v_ego, BRAKE_THRESHOLD_BP, BRAKE_THRESHOLD_V))


def brake_weight(a_brake: float, v_ego: float) -> float:
  """How far into the brake regime a request is: 0 is all gas, 1 is all brake. Decided from the
  accel at the brake lookahead, so braking is anticipated by the longer of the two delays."""
  return float(np.clip((brake_threshold(v_ego) + REGIME_BLEND - a_brake) / (2 * REGIME_BLEND), 0.0, 1.0))


def split_delay_enabled(CP) -> bool:
  """Platforms whose planner uses the learned gas/brake split. Everywhere else longlagd runs in
  shadow mode and the planner keeps a single delay."""
  return CP.brand == "mazda" and bool(CP.flags & MazdaSafetyFlags.GEN2)


def _clip_delay(delay: float) -> float:
  return float(np.clip(delay, MIN_LONG_DELAY, MAX_LONG_DELAY))


def base_long_delay(CP, starpilot_toggles) -> float:
  """The Longitudinal Actuator Delay setting, which is the car's default unless customized."""
  base = getattr(starpilot_toggles, "longitudinalActuatorDelay", CP.longitudinalActuatorDelay)
  if not math.isfinite(base) or base <= 0.0:
    return float(CP.longitudinalActuatorDelay)
  return float(base)


def get_long_delays(CP, starpilot_toggles, live_delay=None) -> tuple[float, float]:
  """(gas, brake) lookaheads for the planner.

  Both are the base delay until longlagd has estimated both regimes on a platform that opts in;
  then the learned values are used, and a customized setting moves their midpoint while keeping
  the learned gap between them."""
  base = base_long_delay(CP, starpilot_toggles)
  gas = brake = base

  learned = (live_delay is not None and split_delay_enabled(CP) and
             live_delay.gas.status == LongDelayStatus.estimated and
             live_delay.brake.status == LongDelayStatus.estimated)
  if learned:
    gas, brake = float(live_delay.gas.delay), float(live_delay.brake.delay)
    if getattr(starpilot_toggles, "use_custom_longitudinalActuatorDelay", False):
      half_gap = (brake - gas) / 2
      gas, brake = base - half_gap, base + half_gap

  return _clip_delay(gas), _clip_delay(brake)
