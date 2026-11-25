"""Optional Mazda camera observation, GEN2 experimental-mode demand (distance stalk, crawl zone) and the GEN2
distance-setting personality; never a longitudinal control grant."""
import time

from opendbc.car.mazda.values import CAR, MazdaFlags
from openpilot.cereal import messaging
from openpilot.starpilot.conditional_mode.policy import ManualIntent
from openpilot.starpilot.conditional_mode.preferences import manual_for_drive

MAX_AGE_NS = 150_000_000
DASHBOARD_CARS = frozenset(CAR)
# CRUZE_STATE DISTANCE_SETTING: 4 closest .. 1 farthest, 0 with main off. The demand sits on 1, the FARTHEST
# setting; it was written as "closest" while the setting's direction was read backwards.
STALK_EXPERIMENTAL_SETTING = 1


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
  """The GEN2 distance buttons (closer and farther, in CRZ_BTNS) step the car's own distance setting.
  Parking it on setting 1 (the farthest) demands experimental mode for as long as it stays there.

  The crawl zone demands it too. openpilot owns the car there under blended ACC, and left to
  itself CEM's slow-lead trigger flips chill/experimental about once a second behind a creeping
  lead (20/min on the 11-19 drives), each flip stepping the plan 0.3-0.5 m/s^2 in one frame."""
  message = _companion(state, message, car_state_stamp)
  message.starpilotCarState.forceExperimental = bool(
    state.canValid and not state.canTimeout and state.cruiseState.available and
    (getattr(parsed, 'distance_setting', 0) == STALK_EXPERIMENTAL_SETTING or getattr(parsed, 'op_crawl', False)))
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


# DISTANCE_SETTING (4 closest .. 1 farthest) -> LongitudinalPersonality (0 aggressive, 1 standard, 2 relaxed).
# The closest setting is where the driver spends most of the time, so it carries the everyday personality.
SETTING_PERSONALITY = {4: 0, 3: 1, 2: 2, 1: 2}


class DistancePersonality:
  """GEN2 openpilot long: the car's distance setting picks the driving personality whenever it changes,
  including the first reading after main comes on. A personality chosen in between (a Wheel Controls
  press at the closest setting, the settings screen) holds until the setting moves again."""

  def __init__(self):
    self.setting = None

  def update(self, cp, parsed, state, params):
    """Returns the personality it saved, or None."""
    if not (gen2(cp) and cp.openpilotLongitudinalControl) or not state.canValid or state.canTimeout:
      return None
    setting = getattr(parsed, 'distance_setting', 0)
    previous, self.setting = self.setting, setting
    personality = SETTING_PERSONALITY.get(setting)
    if personality is None or setting == previous or params.get_bool("SafeMode"):
      return None
    params.put("LongitudinalPersonality", personality)  # selfdrived re-reads it every 0.1 s
    return personality


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
