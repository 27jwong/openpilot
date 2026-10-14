import math

import numpy as np

from opendbc.car.mazda.values import MazdaSafetyFlags

# Bounds on any lookahead the planner will use, learned or set by hand.
MIN_LONG_DELAY = 0.05
MAX_LONG_DELAY = 1.2

# Accel request (m/s^2) below which the car has to apply its brakes rather than just close
# the throttle. It drops with speed because coast-down decel (drag, rolling resistance, engine
# braking) grows with it.
#
# Placed from 86 min of CX-30 driving (tools/tuning/analyze_longitudinal.py threshold sweep).
# The two-path fit was best 0.45 below the old table in 5-15 m/s and 0.35 below it in 15-25 m/s,
# which is -0.75 at both 10 and 20 m/s; above 25 m/s the sweep is flat and can't place it. The
# sweep's r2 is a broad plateau (within 0.004 for offsets of -0.65 to -0.25 from the old table), so
# this is the middle of a range, not a sharp optimum. longlagd learns nothing under 5 m/s (MIN_VEGO), so rather than
# hold -0.75 where there is no data and drag is least, the table ramps back to the old -0.15 at a
# standstill. Only the CX-30 has been measured; on any other platform longlagd runs in shadow
# mode and the planner's crossfade is a no-op (see split_delay_enabled).
BRAKE_THRESHOLD_BP = [0., 10., 20.]
BRAKE_THRESHOLD_V = [-0.15, -0.75, -0.75]

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
  then the learned values are used. A customized setting sets the gas delay, the one nearly all
  driving runs on and the one that setting meant when there was only a single delay, and the brake
  delay keeps its learned gap from it."""
  base = base_long_delay(CP, starpilot_toggles)
  gas = brake = base

  # capnp enums compare equal to their names; keeps cereal out of this module's imports (Galaxy uses it)
  learned = (live_delay is not None and split_delay_enabled(CP) and
             live_delay.gas.status == "estimated" and live_delay.brake.status == "estimated")
  if learned:
    gas, brake = float(live_delay.gas.delay), float(live_delay.brake.delay)
    if getattr(starpilot_toggles, "use_custom_longitudinalActuatorDelay", False):
      gas, brake = base, base + (brake - gas)

  return _clip_delay(gas), _clip_delay(brake)
