"""Optional Mazda camera observation and GEN2 distance-stalk demand; never a longitudinal control grant."""
import time

from opendbc.car.mazda.values import CAR, MazdaFlags
from openpilot.cereal import messaging
from openpilot.starpilot.conditional_mode.policy import ManualIntent
from openpilot.starpilot.conditional_mode.preferences import manual_for_drive

MAX_AGE_NS = 150_000_000
DASHBOARD_CARS = frozenset(CAR)
STALK_EXPERIMENTAL_SETTING = 1  # CRUZE_STATE DISTANCE_SETTING: 1 closest .. 4 farthest, 0 with main off


def capable(cp):
  # Read-only observation scope is separate from active-control/AOL admission.
  # Dom's intentional dashboard commit explicitly covers all six GEN1 IDs.
  return (cp.brand == 'mazda' and cp.carFingerprint in DASHBOARD_CARS and int(cp.flags) == int(MazdaFlags.GEN1) and not cp.notCar)


def gen2(cp):
  return cp.brand == 'mazda' and bool(int(cp.flags) & int(MazdaFlags.GEN2)) and not cp.notCar


def compose(cp, parsed, state, message, *, car_state_stamp):
  if gen2(cp):
    return write_stalk_experimental(parsed, state, message, car_state_stamp=car_state_stamp)
  if not capable(cp):
    return message
  return write_stop_sign(parsed, state, message, car_state_stamp=car_state_stamp)


def _companion(state, message, car_state_stamp):
  if message is None:
    message = messaging.new_message('starpilotCarState', valid=bool(state.canValid), logMonoTime=car_state_stamp)
    message.starpilotCarState.sourceCarStateMonoTime = car_state_stamp
  return message


def write_stalk_experimental(parsed, state, message, *, car_state_stamp):
  """GEN2 has no distance button on the bus, only the setting it cycles. Parking the stalk on
  the closest setting demands experimental mode for as long as it stays there."""
  message = _companion(state, message, car_state_stamp)
  message.starpilotCarState.forceExperimental = bool(
    state.canValid and not state.canTimeout and state.cruiseState.available and
    getattr(parsed, 'distance_setting', 0) == STALK_EXPERIMENTAL_SETTING)
  return message


def stalk_experimental(sm, drive_id, now_ns):
  """The conditional host's manual override while the stalk demands experimental mode, else None
  so the driver's own manual state applies. Held, never saved: leaving the setting ends it."""
  try:
    stamp = int(sm.logMonoTime['starpilotCarState'])
    forced = sm.valid['starpilotCarState'] and sm['starpilotCarState'].forceExperimental
  except (KeyError, AttributeError, TypeError, ValueError):
    return None
  if not forced or type(drive_id) is not int or not 0 < drive_id <= stamp <= now_ns or now_ns - stamp > MAX_AGE_NS:
    return None
  return manual_for_drive(ManualIntent.FORCE_EXPERIMENTAL, drive_id, stamp)


def write_stop_sign(parsed, state, message, *, car_state_stamp):
  message = _companion(state, message, car_state_stamp)
  # Missing or stale optional camera data must not invalidate unrelated evidence.
  message.starpilotCarState.dashboardStopSign = 0
  before = time.monotonic_ns()
  boot = time.clock_gettime_ns(getattr(time, 'CLOCK_BOOTTIME', time.CLOCK_MONOTONIC))
  after = time.monotonic_ns()
  observation = getattr(parsed, 'dashboard_stop_sign', None)
  if (type(observation) is tuple and len(observation) == 2 and type(observation[0]) is bool and
      type(observation[1]) is int and 0 <= after - before <= 1_000_000 and state.canValid and not state.canTimeout):
    age = boot - observation[1]
    stamp = before - age
    if 0 <= age <= MAX_AGE_NS and 0 < stamp <= car_state_stamp:
      message.starpilotCarState.dashboardStopSign = int(observation[0])
  return message
