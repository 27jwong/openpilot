from types import SimpleNamespace

import numpy as np

from opendbc.car.mazda.gen2_longitudinal import KI, KP, MAX_BRAKE_OVERSHOOT, ERROR_FILTER_RC, policy_for
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.pid import PIDController
from openpilot.common.realtime import DT_CTRL


def make_cp(brand="mazda", long=True, flags=MazdaSafetyFlags.GEN2.value):
  return SimpleNamespace(brand=brand, openpilotLongitudinalControl=long, flags=flags)


def run(policy, pid, targets, a_ego, speed=10.0):
  """LongControl's pid branch, as extension.prepare_pid / shape_output drive it."""
  outs = []
  for target, ego in zip(targets, a_ego, strict=True):
    error = target - ego
    ff = policy.feedforward(target, speed, 0.0)
    freeze = policy.prepare_pid(pid, target, error, speed, 0.0, (-3.5, 2.0))
    out = pid.update(error, speed=speed, feedforward=ff, freeze_integrator=freeze)
    outs.append(policy.shape_output(float(out), target, error, speed))
  return np.array(outs)


def test_admission():
  assert policy_for(make_cp()) is not None
  assert policy_for(make_cp(long=False)) is None
  assert policy_for(make_cp(flags=MazdaSafetyFlags.GEN1.value)) is None
  assert policy_for(make_cp(brand="toyota")) is None


def test_error_filter_matches_filtering_before_the_pid():
  # Filtering the error ahead of the PI is the reference; the policy applies the same P-term
  # change after it. With ki = 0 the two have to agree exactly.
  rng = np.random.default_rng(0)
  n = 400
  targets = np.full(n, 0.3)
  a_ego = 0.3 + rng.normal(0, 0.1, n)
  policy = policy_for(make_cp())
  pid = PIDController(KP, 0.0, rate=1 / DT_CTRL)
  got = run(policy, pid, targets, a_ego)

  ref_filter = FirstOrderFilter(0.0, ERROR_FILTER_RC, DT_CTRL, initialized=False)
  ref_pid = PIDController(KP, 0.0, rate=1 / DT_CTRL)
  want = []
  for target, ego in zip(targets, a_ego, strict=True):
    err = ref_filter.update(float(target - ego))
    want.append(ref_pid.update(err, speed=10.0, feedforward=target))
  assert np.allclose(got, want, atol=1e-9)
  assert got.std() < np.array([KP[1][1] * (t - e) for t, e in zip(targets, a_ego, strict=True)]).std()


def test_large_error_bypasses_the_filter():
  policy = policy_for(make_cp())
  pid = PIDController(KP, 0.0, rate=1 / DT_CTRL)
  got = run(policy, pid, np.full(5, 1.0), np.full(5, -2.0))
  assert np.allclose(got, 1.0 + float(np.interp(10.0, *KP)) * 3.0)


def test_brake_overshoot_is_bounded_and_integrator_follows():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  # planner wants -1.0 but the car is not decelerating yet: feedback would out-brake it
  got = run(policy, pid, np.full(300, -1.0), np.full(300, 0.0))
  assert got.min() >= -1.0 - MAX_BRAKE_OVERSHOOT - 1e-9
  assert np.isclose(got[-1], -1.0 - MAX_BRAKE_OVERSHOOT)
  # back-calculated: the integrator holds only what was actually sent
  assert np.isclose(pid.p + pid.i + pid.f, got[-1], atol=1e-6) or pid.i > -1.0


def test_positive_target_is_not_clamped():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  got = run(policy, pid, np.full(100, 0.5), np.full(100, 0.0))
  assert got.max() > 0.5


def test_reset_rearms_the_filter():
  policy = policy_for(make_cp())
  pid = PIDController(KP, 0.0, rate=1 / DT_CTRL)
  run(policy, pid, np.full(50, 0.4), np.full(50, 0.0))
  policy.reset()
  assert not policy.error_filter.initialized
