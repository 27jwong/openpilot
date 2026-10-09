import numpy as np

from openpilot.cereal import custom
from opendbc.car.mazda.values import MazdaSafetyFlags

LongDelayStatus = custom.StarPilotLongitudinalDelay.Status

# Bounds on any lookahead the planner will use.
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
  return CP.brand == "mazda" and bool(int(CP.flags) & MazdaSafetyFlags.GEN2)


def _clip_delay(delay: float) -> float:
  return float(np.clip(delay, MIN_LONG_DELAY, MAX_LONG_DELAY))


def get_long_delays(CP, live_delay=None) -> tuple[float, float]:
  """(gas, brake) lookaheads for the planner.

  Both are the car's longitudinalActuatorDelay until longlagd has estimated both regimes on a
  platform that opts in; then the learned values are used."""
  gas = brake = float(CP.longitudinalActuatorDelay)

  if (live_delay is not None and split_delay_enabled(CP) and
      live_delay.gas.status == LongDelayStatus.estimated and live_delay.brake.status == LongDelayStatus.estimated):
    gas, brake = float(live_delay.gas.delay), float(live_delay.brake.delay)

  return _clip_delay(gas), _clip_delay(brake)
