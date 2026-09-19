import pytest

from opendbc.car import DT_CTRL, structs
from opendbc.car.common.pid import PIDController
from opendbc.car.mazda import longitudinal
from opendbc.car.mazda.longitudinal import MazdaGen2LongitudinalPolicy, policy_for
from opendbc.car.mazda.values import MazdaSafetyFlags


def make_cp(flags=MazdaSafetyFlags.GEN2, brand="mazda", long=True):
  CP = structs.CarParams.new_message()
  CP.brand = brand
  CP.flags = int(flags)
  CP.openpilotLongitudinalControl = long
  CP.longitudinalTuning.deprecated.kpBP = [0.0, 12.0, 30.0]
  CP.longitudinalTuning.deprecated.kpV = [0.2, 0.5, 0.4]
  CP.longitudinalTuning.kiBP = [0.0, 35.0]
  CP.longitudinalTuning.kiV = [1.0, 1.0]
  return CP


def make_policy(blended_acc=False):
  return MazdaGen2LongitudinalPolicy(((0.0, 12.0, 30.0), (0.2, 0.5, 0.4)), blended_acc)


class Context:
  def __init__(self, experimental_mode=False, pitch=None):
    self.experimental_mode = experimental_mode
    self.pitch = pitch


def test_policy_is_mazda_gen2_openpilot_long_only():
  policy = policy_for(make_cp())
  assert isinstance(policy, MazdaGen2LongitudinalPolicy)
  kp_bp, kp_v = policy.kp
  assert kp_bp == pytest.approx((0.0, 12.0, 30.0))
  assert kp_v == pytest.approx((0.2, 0.5, 0.4))  # CarParams stores float32
  assert policy_for(make_cp(flags=MazdaSafetyFlags.GEN1)) is None
  assert policy_for(make_cp(long=False)) is None
  assert policy_for(make_cp(brand="gm")) is None


def test_error_filter_smooths_measurement_noise():
  policy = make_policy()

  # a steady error settles onto the true value rather than being attenuated forever
  for _ in range(200):
    filtered = policy.filter_accel_error(0.4)
  assert filtered == pytest.approx(0.4, abs=1e-3)

  # alternating noise of the size aEgo actually carries is rejected, not passed through
  policy.reset()
  for _ in range(50):
    policy.filter_accel_error(0.4)
  outputs = [policy.filter_accel_error(0.4 + (0.15 if i % 2 else -0.15)) for i in range(40)]
  assert max(outputs) - min(outputs) < 0.10


def test_error_filter_passes_large_errors_immediately():
  policy = make_policy()
  for _ in range(100):
    policy.filter_accel_error(0.0)

  # a hard-braking request must not be delayed by the smoothing
  assert policy.filter_accel_error(-2.5) == pytest.approx(-2.5)
  # and the filter picks up from there instead of snapping back to zero
  assert policy.filter_accel_error(-2.5) == pytest.approx(-2.5, abs=1e-6)


def test_error_filter_resets_with_the_controller():
  policy = make_policy()
  for _ in range(100):
    policy.filter_accel_error(1.0)
  assert policy.accel_error_filter.x > 0.9

  policy.reset()
  assert not policy.accel_error_filter.initialized
  # first sample after a reset is adopted directly, no ramp from a stale value
  assert policy.filter_accel_error(-0.3) == pytest.approx(-0.3)


def test_blended_acc_resets_the_pid_on_the_handover_into_experimental():
  pid = PIDController(0.5, 1.0)
  policy = make_policy(blended_acc=True)
  policy.pre_pid(pid, 0.0, Context(experimental_mode=False))
  pid.i = 0.8  # windup from cycles whose output the stock ACC was overriding
  policy.pre_pid(pid, 0.0, Context(experimental_mode=True))
  assert pid.i == 0.0
  # only the rising edge resets
  pid.i = 0.3
  policy.pre_pid(pid, 0.0, Context(experimental_mode=True))
  assert pid.i == 0.3


def test_handover_reset_is_blended_acc_only():
  pid = PIDController(0.5, 1.0)
  policy = make_policy(blended_acc=False)
  policy.pre_pid(pid, 0.0, Context(experimental_mode=False))
  pid.i = 0.8
  policy.pre_pid(pid, 0.0, Context(experimental_mode=True))
  assert pid.i == 0.8


def test_brake_overshoot_is_bounded():
  """Feedback may lead the plant, but not out-brake the planner without limit."""
  pid = PIDController(0.5, 1.0)
  policy = make_policy()
  cap = longitudinal.MAZDA_GEN2_MAX_BRAKE_OVERSHOOT

  pid.i = -1.2  # a wound-up integrator mid brake
  limited = policy.limit_brake_overshoot(pid, -2.4, -1.5)
  assert limited == pytest.approx(-1.5 - cap)
  # the integrator was back-calculated to what actually got sent, not left wound up
  assert pid.i == pytest.approx(-1.2 + ((-1.5 - cap) - -2.4))


def test_brake_overshoot_leaves_normal_commands_alone():
  pid = PIDController(0.5, 1.0)
  policy = make_policy()
  before = pid.i = -0.3
  # within the allowance: untouched, and the integrator is not disturbed
  assert policy.limit_brake_overshoot(pid, -1.55, -1.5) == -1.55
  assert pid.i == before
  # positive requests are not the braking case at all
  assert policy.limit_brake_overshoot(pid, 0.2, 0.8) == 0.2
  assert pid.i == before


def _hold_pitch(policy, pitch, seconds, v_ego=25.0):
  out = 0.0
  for _ in range(int(seconds / DT_CTRL)):
    out = policy.get_pitch_feedforward(pitch, v_ego, True)
  return out


def _flat_and_settled():
  """A policy whose washout has converged on level ground, as it is after a minute of driving."""
  policy = make_policy()
  _hold_pitch(policy, 0.0, 15.0)
  return policy


def test_pitch_feedforward_answers_a_grade_change():
  """A hill the car has not absorbed yet is fed forward at close to full gravity."""
  policy = _flat_and_settled()
  # cresting onto a 4% upgrade: the washout still reads flat, so the whole step is handed over.
  # 9.81 * sin(0.04) = 0.392
  assert policy.get_pitch_feedforward(0.04, 25.0, True) == pytest.approx(0.392, abs=0.01)
  # and it is still most of the way there a second in, while the car is still catching up
  assert _hold_pitch(policy, 0.04, 1.0) == pytest.approx(0.392 * 0.717, abs=0.02)
  # a downgrade is the mirror image
  policy = _flat_and_settled()
  assert policy.get_pitch_feedforward(-0.04, 25.0, True) == pytest.approx(-0.392, abs=0.01)


def test_pitch_feedforward_does_nothing_on_a_hill_already_underway():
  """Engaging mid-climb must not inject a step: by then the car has absorbed the grade."""
  assert make_policy().get_pitch_feedforward(0.04, 25.0, True) == 0.0


def test_pitch_feedforward_washes_out_on_a_sustained_hill():
  """The CX-30 compensates a steady grade itself, so the term must decay to nothing."""
  policy = _flat_and_settled()
  assert _hold_pitch(policy, 0.04, 3.0) == pytest.approx(0.392 * 0.368, abs=0.02)  # one RC
  assert _hold_pitch(policy, 0.04, 12.0) == pytest.approx(0.0, abs=0.02)


def test_pitch_feedforward_is_bounded():
  policy = _flat_and_settled()
  # a 45 degree "grade" can only come from a broken pitch estimate
  assert policy.get_pitch_feedforward(0.79, 25.0, True) == pytest.approx(longitudinal.MAZDA_GEN2_PITCH_FF_MAX)
  policy = _flat_and_settled()
  assert policy.get_pitch_feedforward(-0.79, 25.0, True) == pytest.approx(-longitudinal.MAZDA_GEN2_PITCH_FF_MAX)


def test_pitch_feedforward_re_arms_instead_of_holding_a_stale_washout():
  """Coming back from creep or a disengage must not step a settled washout into the command."""
  policy = make_policy()
  _hold_pitch(policy, 0.04, 12.0)
  assert policy.get_pitch_feedforward(0.04, 1.0, True) == 0.0      # below the speed floor
  # back above it on the same hill: re-armed, so the term is 0, not -0.39
  assert policy.get_pitch_feedforward(0.04, 25.0, True) == 0.0
  policy.reset()
  _hold_pitch(policy, 0.04, 12.0)
  assert policy.get_pitch_feedforward(0.04, 25.0, False) == 0.0    # not in the PID state
  assert policy.get_pitch_feedforward(0.04, 25.0, True) == 0.0


def test_pitch_feedforward_ignores_a_missing_or_bad_estimate():
  policy = make_policy()
  assert policy.get_pitch_feedforward(None, 25.0, True) == 0.0
  assert policy.get_pitch_feedforward(float("nan"), 25.0, True) == 0.0
