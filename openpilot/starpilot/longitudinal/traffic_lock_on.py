"""Traffic Mode locks on to the car ahead (Mazda GEN2).

Base Traffic Mode blends its follow time and jerk from the Traffic values at a stop toward the *Relaxed*
personality's by 25 m/s. On the CX-30 that made Traffic looser than Aggressive above ~15 mph: route
00000029 4:37-5:38 (Traffic on, 20-30 mph) held a 1.51 s median headway where Aggressive holds ~1.17 s,
because TrafficFollow 0.65 -> RelaxedFollow 2.0 is already 1.3 s at 28 mph, and the jerk had drifted
halfway to Relaxed's 180/150%.

Here Traffic's follow rises from TrafficFollow at a stop to LOCK_ON_FOLLOW at 25 m/s instead (never
looser than base would be), and the driver's Traffic jerk values hold at every speed. Replayed against
that stretch's lead: headway 1.51 -> 1.04 s, and with the Traffic jerk at 50% the response lag to the
lead's speed changes 1.85 -> 1.60 s with a steadier gap (std 3.4 -> 2.7 m).

A custom Traffic following curve from the profile document is the driver's explicit choice and is kept.
Nothing is saved; the tuning is rebuilt every planner cycle.
"""
from dataclasses import replace
import math

from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.starpilot.longitudinal.profile_runtime import ProfileTuning, TrafficSettings, TRAFFIC_BP

LOCK_ON_FOLLOW = 1.0  # s of follow at TRAFFIC_BP[1] (25 m/s), the user's Aggressive low-speed follow


def eligible(cp) -> bool:
  return cp.brand == 'mazda' and bool(cp.openpilotLongitudinalControl) and bool(int(cp.flags) & MazdaSafetyFlags.GEN2.value)


def _blend(points: tuple[float, float], speed: float) -> float:
  ratio = min(max((speed - TRAFFIC_BP[0]) / (TRAFFIC_BP[1] - TRAFFIC_BP[0]), 0.0), 1.0)
  return float(points[0] + (points[1] - points[0]) * ratio)


def traffic_lock_on(cp, tuning: ProfileTuning | None, settings, v_ego: float) -> ProfileTuning | None:
  """Traffic's tuning with the lock-on follow and held Traffic jerk; anything else passes through untouched."""
  if (tuning is None or tuning.personality_id != 'traffic' or not eligible(cp) or
      not isinstance(settings, TrafficSettings) or not settings.valid or len(settings.jerk) != 5 or
      len(settings.follow) != 2 or not math.isfinite(v_ego)):
    return tuning
  traffic_follow = float(settings.follow[0])
  follow = tuning.follow_seconds
  # the base blend is what resolve() produced unless a document curve replaced it
  if abs(follow - _blend(settings.follow, v_ego)) < 1e-6:
    follow = min(follow, _blend((traffic_follow, max(traffic_follow, LOCK_ON_FOLLOW)), v_ego))
  acceleration, deceleration, speed, speed_decrease, danger = (float(points[0]) for points in settings.jerk)
  return replace(tuning, follow_seconds=follow, acceleration_jerk=acceleration, deceleration_jerk=deceleration,
                 speed_jerk=speed, speed_decrease_jerk=speed_decrease, danger_jerk=danger)
