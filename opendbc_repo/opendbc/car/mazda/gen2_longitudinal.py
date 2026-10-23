"""CX-30 (GEN2) longitudinal tune: the PI gains and output shaping identified from its logs."""
import numpy as np

from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import DT_CTRL
from opendbc.car.mazda.values import MazdaSafetyFlags

# The ACC accel interface is unity-gain and linear over +-1.6 m/s^2 against a first-order plant
# with tau ~0.42 s, so the feedforward carries the request and the PI only has to reject
# disturbance. The gains are IMC-tuned against that plant. kp is only usable because the error is
# low-passed first, and it comes back down at highway speed: at 0.8 it turned aEgo noise into a
# 0.3-1.5 Hz back-and-forth behind a lead, 3x what the planner asked for, and tracked no better.
KP = ([0., 12., 30.], [0.2, 0.5, 0.4])
KI = ([0., 35.], [1.0, 1.0])

# aEgo is a differentiated wheel speed, so it carries 0.05 m/s^2 of noise at highway speed and 0.15
# at parking-lot speed. Feeding that straight into kp would dither the pedal, so the error gets a
# short low-pass first. 0.10 s costs ~4% of the tracking improvement and removes ~60% of the command
# jerk the unfiltered term would add.
ERROR_FILTER_RC = 0.10
# Past this the request is a hard stop or a hard launch and the loop should not be spending phase
# margin on smoothing; the raw error goes through instead.
ERROR_FILTER_BYPASS = 1.5

# The PI has to lead the plant to overcome its ~0.4 s lag, but with a real integrator that lead keeps
# building through a sustained brake and the command ends up well below what the planner asked for -
# measured at -0.20 m/s^2 on average during hard braking, which lands as a jab. Bound it to roughly the
# authority the old kp=0/ki=0.1 loop had.
MAX_BRAKE_OVERSHOOT = 0.10


class Gen2LongitudinalPolicy:
  friction_variant = False
  stopping_decel_rate = 1.0
  kp = KP
  ki = KI

  def __init__(self):
    self.pid = None
    self.error_filter = FirstOrderFilter(0.0, ERROR_FILTER_RC, DT_CTRL, initialized=False)
    self.error_delta = 0.0

  def reset(self) -> None:
    self.error_filter.initialized = False
    self.error_delta = 0.0

  def feedforward(self, target: float, speed: float, last_output: float) -> float:
    return target

  def prepare_pid(self, pid, target: float, error: float, speed: float,
                  last_output: float, accel_limits: tuple[float, float], *,
                  should_stop: bool = False, has_lead: bool | None = None) -> bool:
    # LongControl hands the PID the raw error, so the filtered part of it is applied to the output in
    # shape_output instead: the P term is linear in the error, and the integrator averages the noise out.
    self.pid = pid
    if abs(error) > ERROR_FILTER_BYPASS:
      self.error_filter.x = float(error)
      self.error_filter.initialized = True
      self.error_delta = 0.0
    else:
      self.error_delta = float(self.error_filter.update(float(error))) - float(error)
    return False

  def shape_output(self, output: float, target: float, error: float, speed: float) -> float:
    output += float(np.interp(speed, *KP)) * self.error_delta

    # Stop feedback from out-braking the planner by more than a fixed margin. The integrator is
    # back-calculated to whatever was actually sent, so releasing the clamp cannot hand back a step
    # that the driver feels as a second jab.
    if target < 0.0 and self.pid is not None:
      floor = target - MAX_BRAKE_OVERSHOOT
      if output < floor:
        self.pid.i += floor - output
        output = floor
    return output


def policy_for(cp):
  if cp.brand != "mazda" or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return Gen2LongitudinalPolicy()
