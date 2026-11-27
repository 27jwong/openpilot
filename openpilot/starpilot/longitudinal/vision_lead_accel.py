"""A vision-only lead's braking, measured from its own speed track (Mazda: no radar objects on the bus).

radard publishes a camera lead's acceleration as the driving model's own lead accel output, lead.a[0]. On the
CX-30 that output reads only part of real braking: against the slope of the same model's lead speed, confirmed by
its distance track, a[0] is about 30-50% of a lead braking harder than 2 m/s^2. The planner extrapolates aLeadK
whenever the lead brakes harder than 0.5 m/s^2 or is within 7 s of impact (long_mpc build_model_lead_trajectory),
so that under-read is what it plans with exactly when a lead brakes hard.

This keeps a [speed, accel] Kalman filter per lead on the lead's speed. That speed is the model's closing speed
plus the car's wheel speed from 0.2 s earlier: the model's own ego speed trails the wheels by 0.2-0.3 s, and the
newest wheel speed would add every change in the car's own braking to the lead. A new car (a distance jump), a gap
in the frames or a speed step no real car makes restarts the filter, and for its first 0.5 s on a car only a[0] is
published.

The filter's accel is used only while a[0] itself says the lead brakes harder than 0.5 m/s^2, and then at most
4x a[0]. Otherwise a[0] passes through untouched, so the planner's switch between the model's lead path and
extrapolating (also at 0.5) never changes, and neither does any yes/no use of aLeadK at -0.5 or above. Only how hard
an already-braking lead is extrapolated changes.
"""
import math
from collections import deque

import numpy as np

DT = 0.05                      # s, one model frame
SPEED_NOISE = 0.14             # m/s: the vision lead speed's frame-to-frame noise on the CX-30 (models of 2026-10/11)
JERK_NOISE = 0.8               # m/s^3/sqrt(s): how fast a lead's accel is allowed to change; sets the filter's speed
EGO_DELAY_FRAMES = 4           # the model's closing speed lines up with the wheel speed this many frames earlier
WARMUP_FRAMES = 10             # 0.5 s on a car before its filtered accel is trusted
CONFIRM_ACCEL = -0.5           # m/s^2: a[0] must already say the lead brakes; = long_mpc's model-path cutoff
MAX_MODEL_MULTIPLE = 4.0       # never more than this many times the braking a[0] reports
MAX_FRAME_GAP_S = 0.12         # a dropped model frame restarts the filter
JUMP_ABS, JUMP_REL = 4.0, 0.15 # m, fraction: a distance jump this big is another car
MAX_SPEED_STEP = 2.0           # m/s in one frame against the filter's prediction: another car, not braking


def _steady_gains(q: float, r: float, dt: float, iters: int = 200) -> tuple[float, float]:
  """Steady-state Kalman gains of a [speed, accel] filter with white jerk q and speed noise r."""
  F = np.array([[1., dt], [0., 1.]])
  Q = q * q * np.array([[dt ** 3 / 3., dt ** 2 / 2.], [dt ** 2 / 2., dt]])
  P = np.eye(2)
  K = np.zeros(2)
  for _ in range(iters):
    P = F @ P @ F.T + Q
    K = P[:, 0] / (P[0, 0] + r * r)
    P = P - np.outer(K, P[0, :])
  return float(K[0]), float(K[1])


GAINS = _steady_gains(JERK_NOISE, SPEED_NOISE, DT)


class _Track:
  __slots__ = ('mono_s', 'd_rel', 'v', 'a', 'age')

  def __init__(self, mono_s: float, d_rel: float, v: float):
    self.mono_s, self.d_rel, self.v, self.a, self.age = mono_s, d_rel, v, 0.0, 0


class VisionLeadAccel:
  def __init__(self):
    self._v_ego: deque[float] = deque(maxlen=EGO_DELAY_FRAMES + 1)
    self._tracks: dict[int, _Track] = {}

  def observe_ego(self, v_ego: float) -> None:
    """The car's wheel speed, once per model frame, before any update() for that frame."""
    if math.isfinite(v_ego):
      self._v_ego.append(v_ego)

  def reset(self, lead: int) -> None:
    """This lead index has no vision lead this frame."""
    self._tracks.pop(lead, None)

  def update(self, lead: int, mono_s: float, d_rel: float, v_rel: float, a_model: float) -> float:
    """aLeadK to publish for this lead (0: leadOne, 1: leadTwo) this model frame.

    v_rel is the model's raw closing speed (its lead speed minus its own ego speed, before any calibration);
    a_model is the model's lead accel a[0], which is returned unchanged whenever the filter has no say."""
    if not self._v_ego or not all(math.isfinite(x) for x in (mono_s, d_rel, v_rel, a_model)):
      self.reset(lead)
      return a_model
    v_lead = self._v_ego[0] + v_rel
    track = self._tracks.get(lead)
    if track is not None:
      v_pred = track.v + track.a * DT
      if (not 0.0 < mono_s - track.mono_s < MAX_FRAME_GAP_S or
          abs(d_rel - track.d_rel) > max(JUMP_ABS, JUMP_REL * track.d_rel) or abs(v_lead - v_pred) > MAX_SPEED_STEP):
        track = None
    if track is None:
      self._tracks[lead] = _Track(mono_s, d_rel, v_lead)
      return a_model
    innovation = v_lead - v_pred
    track.v = v_pred + GAINS[0] * innovation
    track.a += GAINS[1] * innovation
    track.mono_s, track.d_rel = mono_s, d_rel
    track.age += 1
    if track.age < WARMUP_FRAMES or a_model >= CONFIRM_ACCEL or track.a >= a_model:
      return a_model
    return max(track.a, MAX_MODEL_MULTIPLE * a_model)
