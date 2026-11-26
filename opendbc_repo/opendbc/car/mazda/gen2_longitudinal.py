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

# Below ~5 m/s the CX-30 brakes less than it is asked to. Steady commands over routes 25-29, median aEgo minus
# command: +0.2..+0.45 m/s^2 for -0.8..-2.0 at 1-5 m/s, and below 1 m/s creep adds +0.13..+0.28 even for -0.3..-0.5.
# Above 5 m/s the car does what it is told (+-0.1). The bound above then pinned the command at exactly target -
# 0.10 through the final approach of route 00000029 5:48-5:50.5 (planned -1.9, car -1.35..-1.6), and from 5:52 the
# car crept at 1 mph on -0.3..-0.47 from 2.3 m down to 1.3 m behind a stopped car until the driver braked.
# So the feedforward carries the measured shortfall, and the bound widens there for the PI to fill the rest.
# Both scale with the braking requested and vanish at a zero target, so a target that asks for gas (a launch
# lurch, the 2026-11-19 case below) still gets the plain bound. Replayed through the measured shortfall map, the
# 5:50 stop lands 4.1 m back instead of creeping into 2.1 m, and three other stops land 0.6-1.1 m farther back.
LOW_SPEED_SHORTFALL_BP = [3.5, 5.5]  # m/s: full below, none above
LOW_SPEED_SHORTFALL_GAIN = 0.3  # per m/s^2 of braking asked for
LOW_SPEED_SHORTFALL_MAX = 0.3  # m/s^2
CREEP_SHORTFALL_BP = [1.0, 2.0]  # m/s
CREEP_SHORTFALL = 0.2  # m/s^2, at full braking request
CREEP_SHORTFALL_FULL = 0.5  # m/s^2 of braking asked for at which the creep term is complete
LOW_SPEED_BRAKE_OVERSHOOT = 0.40
LOW_SPEED_BRAKE_OVERSHOOT_BP = [3.0, 6.0]  # m/s
LOW_SPEED_BRAKE_OVERSHOOT_FULL = 0.6  # m/s^2 of braking asked for at which the wider bound is complete


def low_speed_shortfall(target: float, speed: float) -> float:
  """m/s^2 the CX-30 falls short of a braking request at this speed; 0 for a target that is not braking."""
  if target >= 0.0:
    return 0.0
  brake = -target
  low = float(np.interp(speed, LOW_SPEED_SHORTFALL_BP, [1.0, 0.0])) * min(LOW_SPEED_SHORTFALL_MAX, LOW_SPEED_SHORTFALL_GAIN * brake)
  creep = CREEP_SHORTFALL * float(np.interp(speed, CREEP_SHORTFALL_BP, [1.0, 0.0])) * min(1.0, brake / CREEP_SHORTFALL_FULL)
  return low + creep


def brake_overshoot(target: float, speed: float) -> float:
  """How far feedback may brake past the (shortfall-corrected) target."""
  if target >= 0.0:
    return MAX_BRAKE_OVERSHOOT
  wide = float(np.interp(speed, LOW_SPEED_BRAKE_OVERSHOOT_BP, [1.0, 0.0])) * min(1.0, -target / LOW_SPEED_BRAKE_OVERSHOOT_FULL)
  return MAX_BRAKE_OVERSHOOT + (LOW_SPEED_BRAKE_OVERSHOOT - MAX_BRAKE_OVERSHOOT) * wide

# The same lead builds on the gas side whenever the planner steps up faster than the car follows (the car
# answers a command after ~0.15 s plus a 0.4 s lag). With Blended ACC off, the CX-30 drives of 2026-11-24/25
# had 2-3 such steps a drive, mostly where conditional mode dropped back to chill: feedback put the command
# 0.8-1.5 m/s^2 over the planner for 0.5-1.7 s (route 00000028 8:57: target 1.9, command 3.3), and the wound-up
# integrator then kept the car pulling after the target had come back down. Simulated on those drives through
# the fitted plant, 0.30 halves that overshoot (10:49: car peak 2.6 -> 1.9 m/s^2), slightly improves overall
# tracking (RMS 0.111 -> 0.106 m/s^2), and leaves step-ups (median rise 0.66 -> 0.67 s) and launches (2.01 ->
# 2.02 s to 1 m/s) as they were. More gas room than the brake side gets because only gas has to beat the lag
# out of a launch.
MAX_GAS_OVERSHOOT = 0.30

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
    # the error stays target - aEgo; only the command carries the car's low-speed braking shortfall
    return target - low_speed_shortfall(target, speed)

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
    # Below ~5 m/s the margin is measured from the shortfall-corrected command and widens with the braking
    # asked for (LOW_SPEED_SHORTFALL above); both terms are zero at a zero target, so continuity holds.
    if self.pid is not None:
      floor = min(target - low_speed_shortfall(target, speed), 0.0) - brake_overshoot(target, speed)
      if output < floor:
        self.pid.i += floor - output
        output = floor
      # Mirror image on the gas side, also continuous where the target crosses zero.
      ceiling = max(target, 0.0) + MAX_GAS_OVERSHOOT
      if output > ceiling:
        self.pid.i -= output - ceiling
        output = ceiling
    return output


def policy_for(cp):
  if cp.brand != "mazda" or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return Gen2LongitudinalPolicy()
