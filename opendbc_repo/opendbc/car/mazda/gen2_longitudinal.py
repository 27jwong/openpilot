"""CX-30 (GEN2) longitudinal tune: the PI gains and output shaping identified from its logs."""
import numpy as np

from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import DT_CTRL
from opendbc.car.mazda.values import MazdaSafetyFlags
from opendbc.car.structs import car

LongCtrlState = car.CarControl.Actuators.LongControlState

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

# At standstill a planner "go" has to hold this long before openpilot leaves its stop. GEN2 reports
# no cruise standstill under openpilot long, so one frame of not-should-stop is otherwise enough,
# and openpilot owns the standstill outright in the crawl zone. Route 00000022--89e0df5b7f 4:36:
# stopped 1.8 m behind a stopped lead, CEM dropped experimental for 0.25 s while vEgo undershot
# zero, the chill plan read +0.22 for one 50 ms frame (a flat plan returns -previous target), the
# command went to +0.41 and the stop ramp then took 0.5 s to rebuild -0.5, so the car lurched
# forward 0.55 s later. False starts at standstill on the 11-19/11-21 drives were 1-3 planner
# frames (0.04-0.13 s, 4 of 4, each cancelled by the planner itself); real launches say go for
# seconds. Four planner frames covers three at any frame timing and costs 0.19 s per launch. It
# has to be continuous: the flat-plan artifact alternates go/stop frames, so it never accumulates.
STANDSTILL_GO_DWELL_FRAMES = round(0.20 / DT_CTRL)


class Gen2StopPolicy:
  """Holds openpilot's stop at standstill until the planner's go has lasted STANDSTILL_GO_DWELL_FRAMES."""

  def __init__(self):
    self.go_frames = 0

  def reset(self) -> None:
    self.go_frames = 0

  def transition(self, native, previous, active, cs, target, should_stop, evidence):
    # A rolling car still leaves at once: an interrupted launch comes back at 0.3-0.5 m/s.
    leaving = (active and previous == LongCtrlState.stopping and cs.standstill and
               native not in (LongCtrlState.stopping, LongCtrlState.off))
    if not leaving:
      self.go_frames = 0
      return native
    self.go_frames += 1
    return native if self.go_frames >= STANDSTILL_GO_DWELL_FRAMES else LongCtrlState.stopping


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

  def stop_policy(self) -> Gen2StopPolicy:
    return Gen2StopPolicy()

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
    # A target at or above zero asks for no braking at all, so the same margin holds below zero. The
    # floor is then continuous in the target: gating it on target < 0 let feedback brake without bound
    # while the planner asked for gas, and the clamp snapped the command up in one frame (-0.66 to
    # -0.10) the moment the target dipped below zero. That bit after the CX-30's launch lurch and
    # after every stretch of MRCC driving, when the integrator had wound against MRCC's own accel.
    if self.pid is not None:
      floor = min(target, 0.0) - MAX_BRAKE_OVERSHOOT
      if output < floor:
        self.pid.i += floor - output
        output = floor
    return output


def policy_for(cp):
  if cp.brand != "mazda" or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return Gen2LongitudinalPolicy()
