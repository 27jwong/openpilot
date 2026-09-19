"""Mazda GEN2 longitudinal tune, as a vehicle-owned longitudinal policy."""
import math

import numpy as np

from opendbc.car import ACCELERATION_DUE_TO_GRAVITY, DT_CTRL
from opendbc.car.common.filter_simple import FirstOrderFilter
from opendbc.car.mazda.values import MazdaSafetyFlags

# Mazda GEN2 runs a real PI loop on accel error (see mazda/interface.py). aEgo is a
# differentiated wheel speed, so it carries 0.05 m/s^2 of noise at highway speed and
# 0.15 at parking-lot speed. Feeding that straight into kp would dither the pedal, so
# the error gets a short low-pass first. 0.10s costs ~4% of the tracking improvement
# and removes ~60% of the command jerk the unfiltered term would add.
MAZDA_GEN2_ERROR_FILTER_RC = 0.10
# Past this the request is a hard stop or a hard launch and the loop should not be
# spending phase margin on smoothing; hand the raw error through instead.
MAZDA_GEN2_ERROR_FILTER_BYPASS = 1.5

# The PI has to lead the plant to overcome its ~0.4 s lag, but with a real integrator that
# lead keeps building through a sustained brake and the command ends up well below what the
# planner asked for - measured at -0.20 m/s^2 on average during hard braking, which lands as
# a jab. Bound it to roughly the authority the old kp=0/ki=0.1 loop had.
MAZDA_GEN2_MAX_BRAKE_OVERSHOOT = 0.10

# Road grade on the CX-30 is a disturbance with a deadline, not a steady bias. Identified over
# 118 min of longActive on 09-09..09-11 logs, letting both coefficients float:
#
#     aEgo(t) = 0.952 * cmd(t - 0.50 s) - 0.96 * highpass_3s(g * sin(pitch))     R^2 = 0.87
#
# Those coefficients were fit before CP.wheelSpeedFactor was set for the CX-30, so the aEgo
# they were regressed against ran ~4.9% low (non-stock tires). Rescaling into the corrected
# frame puts both at unity - 0.952 * 1.0487 = 1.00 and 0.96 * 1.0487 = 1.01, grade arriving in
# true units from locationd so only aEgo moves - which is what a unity-gain accel interface
# implies, and why PITCH_FF_GAIN below is 1.0 rather than the fitted 0.96. The 0.50 s delay and
# 3 s washout are time constants and carry over unchanged.
#
# The car's own ACC closes an inertial accel loop, so a sustained hill costs the command
# nothing - on a steady 3.5% upgrade at constant speed openpilot averages -0.013 m/s^2, and
# the grade coefficient collapses to -0.145 if the washout is removed. What it does not do is
# react quickly: grade arrives at full unity gain and decays with a ~3 s time constant, and
# for those 3 s the disturbance lands on us. Measured cost over 132 clean upgrade onsets with
# no lead: a mean 1.36 m/s (3.0 mph) sag, p90 2.81 m/s, and aTarget does not ask for any of it
# back until ~3 s in because the planner is reacting to speed error, not to the hill.
#
# So compensate the transient and nothing else. A plain g*sin(pitch) term double-counts against
# the car's own loop and simulated 3x worse than no compensation at all (RMS speed error 0.242
# vs 0.084 m/s); the washed-out form measured 0.020. Over the transitions themselves the washed
# term correlates 0.80 with the acceleration the plant actually fails to deliver.
MAZDA_GEN2_PITCH_FF_WASHOUT_RC = 3.0
MAZDA_GEN2_PITCH_FF_GAIN = 1.0
# Measured |ff|: std 0.087, p99 0.30, max 0.60 m/s^2 over all engaged time, so this clip is a
# bound on a bad pitch estimate rather than a limit the honest signal ever reaches.
MAZDA_GEN2_PITCH_FF_MAX = 0.6
# Below this the wheel-speed aEgo is too coarse for the grade term to mean anything, and creep
# and launch are shaped by their own curves.
MAZDA_GEN2_PITCH_FF_MIN_SPEED = 3.0


class MazdaGen2LongitudinalPolicy:
  """kp, accel error filtering, grade feedforward and brake overshoot bound for GEN2.

  ki stays on CarParams. kp lives in longitudinalTuning.deprecated.kpBP/kpV, which is where
  interface.py still records the tune, since only vehicle policies supply kp now."""
  friction_variant = False
  stopping_decel_rate = 1.0

  def __init__(self, kp, blended_acc: bool):
    self.kp = kp
    self.blended_acc = blended_acc
    self.experimental_mode_last = False
    self.pitch = None
    self.pid = None
    self.reset()

  def reset(self) -> None:
    self.accel_error_filter = FirstOrderFilter(0.0, MAZDA_GEN2_ERROR_FILTER_RC, DT_CTRL, initialized=False)
    self.pitch_washout = FirstOrderFilter(0.0, MAZDA_GEN2_PITCH_FF_WASHOUT_RC, DT_CTRL, initialized=False)

  def pre_pid(self, pid, error: float, context) -> float:
    """Runs ahead of the PID each cycle in the PID state, and returns the error it should see."""
    self.pid = pid
    self.pitch = getattr(context, "pitch", None)

    # Blended ACC hands longitudinal back and forth between the stock ACC and openpilot
    # at the experimental mode boundary. Reset on the handover so the PID doesn't inherit
    # windup from the cycles where its output was being discarded.
    experimental_mode = bool(getattr(context, "experimental_mode", False))
    if self.blended_acc and experimental_mode and not self.experimental_mode_last:
      pid.reset()
      self.reset()
    self.experimental_mode_last = experimental_mode

    return self.filter_accel_error(error)

  def prepare_pid(self, pid, target: float, error: float, speed: float, last_output: float,
                  accel_limits: tuple[float, float], *, should_stop: bool = False, has_lead: bool | None = None) -> bool:
    return False

  def feedforward(self, target: float, speed: float, last_output: float) -> float:
    return target + self.get_pitch_feedforward(self.pitch, speed, True)

  def shape_output(self, output: float, target: float, error: float, speed: float) -> float:
    return self.limit_brake_overshoot(self.pid, output, target) if self.pid is not None else output

  def get_pitch_feedforward(self, pitch, v_ego, active):
    """Feedforward only the part of the road grade the car has not absorbed yet.

    The washout is what makes this safe to add: the CX-30 compensates a sustained grade
    itself, so the term has to decay to zero on the same ~3 s the car takes to catch up,
    or it fights a loop that has already won and the integrator has to unwind it. Returns
    0 whenever the pitch is unusable, so a dropout costs nothing more than today."""
    if not active or pitch is None or not math.isfinite(pitch) or v_ego < MAZDA_GEN2_PITCH_FF_MIN_SPEED:
      # Re-arm rather than hold: a stale washout would step into the command on re-engage.
      self.pitch_washout.initialized = False
      return 0.0

    grade_accel = ACCELERATION_DUE_TO_GRAVITY * math.sin(float(pitch))
    settled = self.pitch_washout.update(grade_accel)
    return float(np.clip((grade_accel - settled) * MAZDA_GEN2_PITCH_FF_GAIN,
                         -MAZDA_GEN2_PITCH_FF_MAX, MAZDA_GEN2_PITCH_FF_MAX))

  def filter_accel_error(self, error):
    """Low-pass the accel error before it reaches the PI, so kp tracks the car and
    not the noise in aEgo. Large errors bypass the filter to keep the loop prompt."""
    if abs(error) > MAZDA_GEN2_ERROR_FILTER_BYPASS:
      self.accel_error_filter.x = float(error)
      self.accel_error_filter.initialized = True
      return error

    return float(self.accel_error_filter.update(float(error)))

  def limit_brake_overshoot(self, pid, output_accel, a_target):
    """Stop feedback from out-braking the planner by more than a fixed margin.

    The integrator is back-calculated to whatever was actually sent, so releasing the
    clamp cannot hand back a step that the driver feels as a second jab."""
    if a_target >= 0.0:
      return output_accel

    floor = a_target - MAZDA_GEN2_MAX_BRAKE_OVERSHOOT
    if output_accel >= floor:
      return output_accel

    pid.i += floor - output_accel
    return floor


def _blended_acc_enabled() -> bool:
  try:
    from openpilot.common.params import Params
    return Params().get_bool("BlendedACC")
  except Exception:
    return False


def policy_for(cp) -> MazdaGen2LongitudinalPolicy | None:
  if cp.brand != "mazda" or not (int(cp.flags) & MazdaSafetyFlags.GEN2) or not cp.openpilotLongitudinalControl:
    return None
  tuning = cp.longitudinalTuning.deprecated
  kp = (tuple(tuning.kpBP), tuple(tuning.kpV)) if len(tuning.kpBP) else 0.0
  return MazdaGen2LongitudinalPolicy(kp, _blended_acc_enabled())
