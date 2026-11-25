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

The bias is a steady-following error only. While the gap is really closing faster than ~1.5 m/s, or the
lead is slowing, the same models read the closing speed too LOW, not too high (2026-11-24, six models,
1393 windows at 27+ mph): a full correction there made the planner see an approach ~0.35 m/s slower than
it was. So the correction also fades with the lead's closing speed and decel over its last second.
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
TREND_TAU_S = 1.0               # s: the closing and slowing fades judge the lead on its last second, not one frame
CLOSING_FADE_BP = (0.5, 1.5)    # m/s of corrected closing speed: full correction below 0.5, none above 1.5
SLOWING_FADE_BP = (-0.5, -0.1)  # m/s^2 of lead accel: none while the lead slows at 0.5, full from -0.1 up


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
    self._trend: dict[int, tuple[float, float, float, float]] = {}  # lead index -> (mono_s, d_rel, v_rel, a_lead)
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

  def _lead_trend(self, lead: int, mono_s: float, d_rel: float, v_rel: float, a_lead: float) -> tuple[float, float]:
    """This lead's closing speed and accel over about the last second; a new car or a gap starts over."""
    last = self._trend.get(lead)
    if last is None or not 0.0 < mono_s - last[0] < MAX_FRAME_GAP_S or abs(d_rel - last[1]) > max(4.0, 0.15 * last[1]):
      v_trend, a_trend = v_rel, a_lead
    else:
      k = min((mono_s - last[0]) / TREND_TAU_S, 1.0)
      v_trend, a_trend = last[2] + k * (v_rel - last[2]), last[3] + k * (a_lead - last[3])
    self._trend[lead] = (mono_s, d_rel, v_trend, a_trend)
    return v_trend, a_trend

  def correction(self, lead: int, mono_s: float, v_ego: float, d_rel: float, v_rel: float, a_lead: float) -> float:
    """m/s to subtract from the model's closing speed for this lead (0: leadOne, 1: leadTwo), right now.

    Call once per model frame for each lead, so its trend follows that lead."""
    if not all(math.isfinite(x) for x in (mono_s, v_ego, d_rel, v_rel, a_lead)):
      self._trend.pop(lead, None)
      return 0.0
    v_trend, a_trend = self._lead_trend(lead, mono_s, d_rel, v_rel, a_lead)
    bias = self.bias(v_ego, d_rel)
    closing = max(-v_rel, 0.0)
    ttc = d_rel / closing if closing > 0.1 else math.inf
    corrected_closing = max(bias - v_trend, 0.0)  # what the planner would see closing, over the last second
    weight = (_ramp(v_ego, APPLY_SPEED_BP) * _ramp(ttc, URGENT_TTC_BP) * _ramp(a_lead, URGENT_ACCEL_BP) *
              (1.0 - _ramp(corrected_closing, CLOSING_FADE_BP)) * _ramp(a_trend, SLOWING_FADE_BP))
    return weight * bias

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
