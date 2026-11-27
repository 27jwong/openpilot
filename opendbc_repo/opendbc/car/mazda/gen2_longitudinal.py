"""CX-30 (GEN2) longitudinal tune: the PI gains and output shaping identified from its logs."""
import math

import numpy as np

from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import DT_CTRL
from opendbc.car.mazda.values import MazdaSafetyFlags
from opendbc.car.structs import car

LongCtrlState = car.CarControl.Actuators.LongControlState

# Where the CX-30's ACC starts using the friction brakes (and lights the brake lamps) instead of engine
# braking: logistic fit of ACC_2 (0x222) BRAKE_RELATED against the sent command, route 00000029 (P = 0.5;
# P goes 0.1 -> 0.9 over +-0.2 m/s^2 around it). More speed means more drag and more room before the brakes;
# an uphill adds room, a downhill takes it away. Flat: -0.15 at 8 m/s, -0.26 at 15, -0.40 at 25, -0.51 at 33.
FRICTION_BRAKE_ONSET = (-0.039, -0.0144, -0.436)  # m/s^2, per m/s, per m/s^2 of g*sin(pitch)


def friction_brake_onset(speed: float, pitch: float) -> float:
  """Commanded accel below which the CX-30 brakes with the friction brakes rather than the engine."""
  c0, c_speed, c_grade = FRICTION_BRAKE_ONSET
  return c0 + c_speed * speed + c_grade * 9.81 * math.sin(pitch)

# The ACC accel interface is unity-gain and linear over +-1.6 m/s^2 against a first-order plant
# with tau ~0.42 s, so the feedforward carries the request and the PI only has to reject
# disturbance. The gains are IMC-tuned against that plant. kp is only usable because the error is
# low-passed first, and it comes back down at highway speed: at 0.8 it turned aEgo noise into a
# 0.3-1.5 Hz back-and-forth behind a lead, 3x what the planner asked for, and tracked no better.
# Cruising, the planner closes its own speed loop around this one (cruise accel = speed error, per
# second over the set speed), so the integrator is a second speed loop on top of it; with the car's
# ~0.55 s lag the two left a resonance near 0.4 Hz. Route 00000029, no-lead cruise at 45-75 mph: the
# phone IMU follows the command by 0.26 s (r 0.77) and the feedback reacts to aEgo 0.18 s later with
# the opposite sign (r -0.93), a ~0.4-1 Hz rocking 30% above MRCC's at 54-81 mph (0.041 vs 0.031 m/s^2,
# 0.3-1.5 Hz). Replayed with both loops closed, ki 0.3 and kp 0.25 at speed take ~10% off the felt
# 0.2-1.5 Hz rocking for 0.02 m/s more speed error; below 8 m/s, where launches and stops live, the
# gains are as they were.
KP = ([0., 12., 30.], [0.2, 0.5, 0.25])
KI = ([0., 8., 12., 35.], [1.0, 1.0, 0.3, 0.3])

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

# The bound above exists for the PI's lead over the car's lag, so it can't tell that lead from a car that
# really isn't braking as hard as it was told. Route 0000002b 2:37.3-2:39.0 (24 mph, lead slowing to a stop):
# cresting a hill the car delivered -0.4..-0.6 for -1.13..-1.26 sent, 0.5-0.6 m/s^2 short of what its own
# response to those commands predicts, and the command sat at exactly target - 0.10 for 1.5 s. The driver
# braked at 2:41.5 with the lead 5.6 m ahead; replayed without the driver, the car stops 1.4 m short of it.
# Widening the bound alone barely helps: at speed the integrator takes ~3 s to absorb a step like that (the
# gains above are low there on purpose). So, as at low speed, the feedforward carries the shortfall, here as
# measured rather than mapped: how far aEgo falls short of the car's known response to what was actually sent,
# a 0.15 s dead time and a 0.4 s lag (fitted against aEgo, the same plant the gains were tuned on). While the car
# follows that response, which is all the PI's lead over the lag is, nothing changes and the bound holds.
# Replayed with both loops closed from 2:37 (the car's measured shortfall added back in), the stop lands 3.1 m
# back instead of 1.4. Whole drives 00000029, 0000002a and 0000002b: the felt jabs (car 0.25+ past the plan for
# 0.2 s+) stay at 4, 4 and 6 and the time 0.2+ past it at 3.8 -> 3.9, 3.6 and 7.8 s. A flat 0.40 bound took
# 2:40 to 4.1 m but went to 9, 6 and 7 jabs.
RESPONSE_DEAD_FRAMES = round(0.15 / DT_CTRL)
RESPONSE_LAG = 0.40  # s
RESPONSE_WARMUP_FRAMES = round(0.60 / DT_CTRL)  # the dead time and most of a lag before the model is trusted
SHORTFALL_RC = 0.30  # s: aEgo noise is 0.05-0.15 m/s^2 per frame
SHORTFALL_DEADBAND = 0.10  # m/s^2: model error left over at steady braking (median -0.06, p95 +0.10-0.16)
SHORTFALL_COMPENSATION_MAX = 0.50  # m/s^2
# Below this the mapped low-speed terms above own the braking: stops, creep and launch lurches were tuned there.
SHORTFALL_SPEED_BP = [4.0, 6.0]  # m/s


class CarResponse:
  """What aEgo should be given the commands actually sent, and how far the car is falling short of it."""

  def __init__(self):
    self.reset()

  def reset(self) -> None:
    self.expected: list[float] = []
    self.model = 0.0
    self.shortfall = FirstOrderFilter(0.0, SHORTFALL_RC, DT_CTRL, initialized=False)
    self.frames = 0
    self.returned: float | None = None

  def update(self, sent: float, a_ego: float, expected_delivery: float | None) -> None:
    """One control frame: sent is last frame's command, expected_delivery what the car should make of it."""
    if self.returned is None or expected_delivery is None or not abs(sent - self.returned) < 1e-6:
      # first frame, or frames this loop didn't shape (stop, start) or a command it didn't send: start over
      self.reset()
      self.model = float(a_ego)
      self.shortfall.x = 0.0
      self.shortfall.initialized = True
      return
    self.expected.append(float(expected_delivery))
    del self.expected[:-(RESPONSE_DEAD_FRAMES + 1)]
    if len(self.expected) > RESPONSE_DEAD_FRAMES:
      self.model += DT_CTRL / RESPONSE_LAG * (self.expected[0] - self.model)
    self.frames += 1
    self.shortfall.update(float(a_ego) - self.model)

  def compensation(self, target: float, speed: float) -> float:
    """m/s^2 of extra braking to send for the car's measured shortfall, while braking is asked for."""
    if target >= 0.0 or self.frames < RESPONSE_WARMUP_FRAMES:
      return 0.0
    short = min(max(float(self.shortfall.x) - SHORTFALL_DEADBAND, 0.0), SHORTFALL_COMPENSATION_MAX)
    return short * min(1.0, -target / LOW_SPEED_BRAKE_OVERSHOOT_FULL) * float(np.interp(speed, SHORTFALL_SPEED_BP, [0.0, 1.0]))

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
    self.response = CarResponse()
    self.accel_limits = (-math.inf, math.inf)
    self.expected_delivery: float | None = None

  def reset(self) -> None:
    self.error_filter.initialized = False
    self.error_delta = 0.0
    self.response.reset()
    self.expected_delivery = None

  def stop_policy(self) -> Gen2StopPolicy:
    return Gen2StopPolicy()

  def feedforward(self, target: float, speed: float, last_output: float) -> float:
    # the error stays target - aEgo; only the command carries the car's braking shortfall
    return target - low_speed_shortfall(target, speed) - self.response.compensation(target, speed)

  def prepare_pid(self, pid, target: float, error: float, speed: float,
                  last_output: float, accel_limits: tuple[float, float], *,
                  should_stop: bool = False, has_lead: bool | None = None) -> bool:
    # LongControl hands the PID the raw error, so the filtered part of it is applied to the output in
    # shape_output instead: the P term is linear in the error, and the integrator averages the noise out.
    self.pid = pid
    self.accel_limits = accel_limits
    self.response.update(float(last_output), float(target) - float(error), self.expected_delivery)
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
    # The measured shortfall (CarResponse above) moves it the same way, and is zero at a zero target too.
    shortfall = low_speed_shortfall(target, speed)
    if self.pid is not None:
      floor = min(target - shortfall - self.response.compensation(target, speed), 0.0) - brake_overshoot(target, speed)
      if output < floor:
        self.pid.i += floor - output
        output = floor
      # Mirror image on the gas side, also continuous where the target crosses zero.
      ceiling = max(target, 0.0) + MAX_GAS_OVERSHOOT
      if output > ceiling:
        self.pid.i -= output - ceiling
        output = ceiling
    # what LongControl will send (it clips to the same limits), and what the car should make of it
    self.response.returned = min(max(output, self.accel_limits[0]), self.accel_limits[1])
    self.expected_delivery = self.response.returned + shortfall
    return output


def policy_for(cp):
  if cp.brand != "mazda" or not cp.openpilotLongitudinalControl or not (int(cp.flags) & MazdaSafetyFlags.GEN2.value):
    return None
  return Gen2LongitudinalPolicy()
