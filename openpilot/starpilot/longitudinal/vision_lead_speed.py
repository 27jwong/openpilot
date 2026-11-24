"""Online calibration of a vision-only lead's closing speed, whichever driving model is running.

radard builds a vision lead's closing speed as the model's lead speed minus the model's own ego speed.
Those are two separate model outputs, and on the CX-30 they disagree with the model's own distance
track: following steadily at highway speed, every model logged so far reports the lead closing at
0.3-1.0 m/s while the distance it reports does not change (measured 2026-11-24 on five models). The
planner then treats the lead as slower than it is and holds 3-10 m more gap than the follow setting.

Over any stretch spent on one car, the true mean closing speed is exactly how fast the gap changed.
This learns the difference as bias(v_ego, d_rel) = theta . [1, (v - 25) / 10, (d - 35) / 20], with a
Kalman filter on theta fed one 5 s window at a time. It starts at zero every drive and re-learns for
whatever model is loaded; no model's numbers are built in. Corrections are capped, ramp in only above
~20 mph, and fade out while the lead is closing in or slowing, so an urgent situation is always seen
exactly as the model reports it.
"""
import math

import numpy as np

WINDOW_FRAMES = 100             # 5 s at 20 Hz
MAX_FRAME_GAP_S = 0.12          # a dropped model frame ends the window
LEARN_MIN_SPEED = 3.0           # m/s
LEARN_MAX_DISTANCE = 80.0       # m: beyond this the distance itself is too noisy to calibrate against
LEARN_MAX_X_STD = 4.0           # m
LEARN_MIN_PROB = 0.6
MAX_WINDOW_ERROR = 3.0          # m/s: a window this far off is a target mix-up, not calibration
WINDOW_NOISE = 0.45             # m/s: one window's closing-speed error after the 5 s fit
PRIOR_STD = (0.5, 0.3, 0.3)     # around zero: no correction until windows say otherwise
DRIFT_STD = (0.01, 0.006, 0.006)  # per window, so the fit can follow a change over ~10 min of following
CORRECTION_CAP = 1.0            # m/s, either direction
APPLY_SPEED_BP = (8.0, 12.0)    # m/s: none at crawl and stop-and-go speeds, full above ~27 mph
URGENT_TTC_BP = (5.0, 12.0)     # s: no correction at 5 s to impact, full beyond 12 s
URGENT_ACCEL_BP = (-1.2, -0.4)  # m/s^2 of lead accel: no correction while the lead brakes harder than 1.2


def _features(v_ego: float, d_rel: float) -> np.ndarray:
  v = min(max(v_ego, 0.0), 40.0)
  d = min(max(d_rel, 5.0), LEARN_MAX_DISTANCE)
  return np.array([1.0, (v - 25.0) / 10.0, (d - 35.0) / 20.0])


def _ramp(x: float, bp: tuple[float, float]) -> float:
  return min(max((x - bp[0]) / (bp[1] - bp[0]), 0.0), 1.0)


class VisionLeadSpeedCalibration:
  def __init__(self):
    self.theta = np.zeros(3)
    self.P = np.diag(np.square(PRIOR_STD))
    self.Q = np.diag(np.square(DRIFT_STD))
    self.windows = 0
    self._clear()

  def _clear(self) -> None:
    self._t: list[float] = []
    self._d: list[float] = []
    self._v_rel: list[float] = []
    self._v_ego: list[float] = []

  def bias(self, v_ego: float, d_rel: float) -> float:
    """Learned closing-speed bias at this speed and gap, m/s (negative: the model reads the lead too slow)."""
    value = float(_features(v_ego, d_rel) @ self.theta)
    return min(max(value, -CORRECTION_CAP), CORRECTION_CAP) if math.isfinite(value) else 0.0

  def correction(self, v_ego: float, d_rel: float, v_rel: float, a_lead: float) -> float:
    """m/s to subtract from the model's closing speed for this lead, right now."""
    if not all(math.isfinite(x) for x in (v_ego, d_rel, v_rel, a_lead)):
      return 0.0
    closing = max(-v_rel, 0.0)
    ttc = d_rel / closing if closing > 0.1 else math.inf
    weight = _ramp(v_ego, APPLY_SPEED_BP) * _ramp(ttc, URGENT_TTC_BP) * _ramp(a_lead, URGENT_ACCEL_BP)
    return weight * self.bias(v_ego, d_rel)

  def observe(self, mono_s: float, prob: float, d_rel: float, v_rel: float, v_ego: float, x_std: float) -> bool:
    """One model frame of the primary lead (v_rel is lead speed minus model ego speed). True when a window lands."""
    usable = (all(math.isfinite(x) for x in (mono_s, prob, d_rel, v_rel, v_ego, x_std)) and prob >= LEARN_MIN_PROB and
              v_ego >= LEARN_MIN_SPEED and 0.0 < d_rel <= LEARN_MAX_DISTANCE and x_std <= LEARN_MAX_X_STD)
    if not usable:
      self._clear()
      return False
    # The identity only holds on one car sampled continuously: a jump to another car, or a gap, starts over.
    if self._d and (abs(d_rel - self._d[-1]) > max(4.0, 0.15 * self._d[-1]) or
                    not 0.0 < mono_s - self._t[-1] < MAX_FRAME_GAP_S):
      self._clear()
    self._t.append(mono_s)
    self._d.append(d_rel)
    self._v_rel.append(v_rel)
    self._v_ego.append(v_ego)
    if len(self._t) < WINDOW_FRAMES:
      return False

    t = np.asarray(self._t) - self._t[0]
    gap_rate = float(np.polyfit(t, np.asarray(self._d), 1)[0])
    error = float(np.mean(self._v_rel)) - gap_rate
    phi = _features(float(np.mean(self._v_ego)), float(np.mean(self._d)))
    self._clear()
    if abs(error) > MAX_WINDOW_ERROR:
      return False

    self.P = self.P + self.Q
    gain = self.P @ phi / (float(phi @ self.P @ phi) + WINDOW_NOISE ** 2)
    self.theta = self.theta + gain * (error - float(phi @ self.theta))
    self.P = self.P - np.outer(gain, phi @ self.P)
    self.windows += 1
    return True
