"""Display-only wheel colors from caller-qualified vehicle observations."""

from dataclasses import dataclass
import math
from typing import Any


BRAKE_RGB = (255, 0, 0)
ACCEL_RGB = (22, 127, 64)


@dataclass(frozen=True)
class WheelFeedback:
  brake_pressed: bool | None = None
  regen_braking: bool | None = None
  gas_pressed: bool | None = None
  acceleration: float | None = None
  commanded_acceleration: float | None = None
  commanded_gas: float | None = None


_EMPTY_FEEDBACK = WheelFeedback()


def _boolean(value: Any) -> bool | None:
  return value if type(value) is bool else None


def _finite(value: Any) -> float | None:
  return float(value) if type(value) in (int, float) and math.isfinite(value) else None


def observe_wheel_feedback(car_state: Any, car_control: Any) -> WheelFeedback:
  """Inputs must already satisfy transport freshness and current-drive checks."""
  if (car_state is None or getattr(car_state, "canValid", None) is not True or
      getattr(car_state, "canTimeout", None) is not False):
    return _EMPTY_FEEDBACK
  actuators = getattr(car_control, "actuators", None) if getattr(car_control, "longActive", None) is True else None
  return WheelFeedback(
    brake_pressed=_boolean(getattr(car_state, "brakePressed", None)),
    regen_braking=_boolean(getattr(car_state, "regenBraking", None)),
    gas_pressed=_boolean(getattr(car_state, "gasPressed", None)),
    acceleration=_finite(getattr(car_state, "aEgo", None)),
    commanded_acceleration=_finite(getattr(actuators, "accel", None)),
    commanded_gas=_finite(getattr(actuators, "gas", None)),
  )


def _wheel_feedback_state(value: WheelFeedback, previous: int = 0) -> int:
  brake_accel, brake_command = (0.15, 0.02) if previous < 0 else (0.25, 0.05)
  gas_accel, gas_command = (0.15, 0.02) if previous > 0 else (0.25, 0.05)
  if (value.brake_pressed is True or value.regen_braking is True or
      value.acceleration is not None and value.acceleration < -brake_accel or
      value.commanded_acceleration is not None and value.commanded_acceleration < -brake_command):
    return -1
  if (value.gas_pressed is True or value.acceleration is not None and value.acceleration > gas_accel or
      value.commanded_acceleration is not None and value.commanded_acceleration > gas_command or
      value.commanded_gas is not None and value.commanded_gas > gas_command):
    return 1
  return 0


def wheel_feedback_rgb(value: WheelFeedback, enabled: bool) -> tuple[int, int, int] | None:
  state = _wheel_feedback_state(value) if enabled else 0
  return BRAKE_RGB if state < 0 else ACCEL_RGB if state > 0 else None


class WheelFeedbackFade:
  """Hysteresis rejects threshold chatter; one signed fade keeps both hues clear."""

  def __init__(self) -> None:
    self._target = 0
    self._amount = 0.0
    self._last_ns = 0
    self._drive_frame: int | None = None

  def update(self, value: WheelFeedback, enabled: bool, now_ns: int, drive_frame: int | None) -> tuple[int, int, int]:
    # Untimed snapshots are static layout samples.
    if now_ns == 0:
      return wheel_feedback_rgb(value, enabled) or (255, 255, 255)
    if (not self._last_ns or not enabled or value == _EMPTY_FEEDBACK or
        drive_frame != self._drive_frame or now_ns < self._last_ns):
      self._target = 0
      self._amount = 0.0
      self._last_ns = now_ns
    dt = (now_ns - self._last_ns) / 1e9
    self._last_ns, self._drive_frame = now_ns, drive_frame
    self._target = _wheel_feedback_state(value, self._target) if enabled else 0
    self._amount += (self._target - self._amount) * (1 - math.exp(-dt / 0.08))
    endpoint = BRAKE_RGB if self._amount < 0 else ACCEL_RGB
    red, green, blue = (round(255 + (channel - 255) * abs(self._amount)) for channel in endpoint)
    return red, green, blue
