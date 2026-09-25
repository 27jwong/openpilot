"""Default Silverado acceleration filtering and integral recovery."""

import numpy as np

from opendbc.car import DT_CTRL
from opendbc.car.gm.longitudinal import GMOrdinaryLongitudinalPolicy


class GMTruckLongitudinalPolicy(GMOrdinaryLongitudinalPolicy):
  kp = ((0.0, 5.0, 15.0, 35.0), (0.02, 0.03, 0.028, 0.022))

  def __init__(self):
    self.kp = (self.kp[0], tuple(float(np.float32(value)) for value in self.kp[1]))
    self.reset()

  def reset(self):
    self.filtered_target = None

  def target(self, target: float, speed: float, should_stop: bool) -> float:
    if (self.filtered_target is None or speed < 12.0 or should_stop or target <= -0.65 or
        target < self.filtered_target - 0.45):
      self.filtered_target = float(target)
    else:
      tau = 0.14 if target < self.filtered_target else 0.20
      self.filtered_target += DT_CTRL / (tau + DT_CTRL) * (float(target) - self.filtered_target)
    return self.filtered_target

  def prepare_pid(self, pid, target, error, speed, last_output, accel_limits, *, should_stop=False, has_lead=None):
    super().prepare_pid(pid, target, error, speed, last_output, accel_limits,
                        should_stop=should_stop, has_lead=has_lead)

    light_target = float(np.interp(speed, (8.0, 15.0, 25.0), (0.03, 0.06, 0.10)))
    if (pid.i > 0.0 and last_output > 0.10 and target <= light_target and
        not (speed <= 0.35 and target > -0.40) and
        (last_output - max(target, 0.0) > 0.08 or error <= -0.08)):
      factor = float(np.interp(target, (-0.30, -0.10, -0.02, light_target), (0.20, 0.35, 0.60, 0.98)))
      if error < -0.20:
        factor *= 0.75
      pid.i *= factor

    if pid.i < -0.02 and speed >= 12.0 and target > -0.85 and error > 0.04:
      mismatch = float(target) - float(last_output)
      if mismatch > 0.10:
        release = float(np.interp(max(mismatch, error), (0.10, 0.25, 0.50), (0.0008, 0.0020, 0.0040)))
        pid.i = min(0.0, pid.i + release)
    return False
