from types import SimpleNamespace

import numpy as np

from opendbc.car.mazda.gen2_longitudinal import (KI, KP, MAX_BRAKE_OVERSHOOT, ERROR_FILTER_RC, STANDSTILL_GO_DWELL_FRAMES,
                                                 LongCtrlState, policy_for)
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


def test_brake_floor_holds_for_a_target_asking_for_gas():
  # CX-30 2026-11-19, 14:42: the car's launch lurch read +1.5..1.7 m/s^2 against a +0.06 target, and with
  # the floor gated on target < 0 the integrator wound the command down to -0.67
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  got = run(policy, pid, np.full(100, 0.06), np.full(100, 1.6), speed=0.5)
  assert got.min() >= -MAX_BRAKE_OVERSHOOT - 1e-9


def test_brake_floor_is_continuous_where_the_target_crosses_zero():
  # the same windup, then the target dips just below zero: the old clamp switched on in that frame and
  # snapped the command from -0.66 to -0.10
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  targets = np.concatenate([np.full(100, 0.01), np.full(20, -0.01)])
  got = run(policy, pid, targets, np.full(120, 0.8), speed=0.8)
  assert np.abs(np.diff(got)).max() < 0.02


def test_reset_rearms_the_filter():
  policy = policy_for(make_cp())
  pid = PIDController(KP, 0.0, rate=1 / DT_CTRL)
  run(policy, pid, np.full(50, 0.4), np.full(50, 0.0))
  policy.reset()
  assert not policy.error_filter.initialized


def stop_states(stop, goes, *, standstill=True, active=True):
  """Gen2StopPolicy fed LongControl's native transition: leaving stopping whenever the plan says go."""
  cs = SimpleNamespace(standstill=standstill)
  state, out = LongCtrlState.stopping, []
  for go in goes:
    native = (LongCtrlState.off if not active else
              LongCtrlState.pid if go else LongCtrlState.stopping if state == LongCtrlState.stopping else state)
    state = stop.transition(native, state, active, cs, 0.22 if go else -0.1, not go, None)
    out.append(state)
  return out


def test_standstill_go_blip_keeps_the_stop():
  # route 00000022--89e0df5b7f 4:36: one 50 ms planner frame of go at standstill, 1.8 m behind a stopped lead
  stop = policy_for(make_cp()).stop_policy()
  goes = [False] * 20 + [True] * 6 + [False] * 30
  assert set(stop_states(stop, goes)) == {LongCtrlState.stopping}
  # route 00000016--47046e6bc8 45:59.7: three planner frames of go (0.13 s) that the planner cancelled
  # itself; three frames can span 16 control frames
  goes = [False] * 20 + [True] * 16 + [False] * 30
  assert set(stop_states(stop, goes)) == {LongCtrlState.stopping}


def test_alternating_go_frames_never_add_up():
  # the flat-plan artifact (target = -previous target) flips go/stop every planner frame
  stop = policy_for(make_cp()).stop_policy()
  goes = ([True] * 5 + [False] * 5) * 10
  assert set(stop_states(stop, goes)) == {LongCtrlState.stopping}


def test_held_go_leaves_the_stop_after_the_dwell():
  stop = policy_for(make_cp()).stop_policy()
  states = stop_states(stop, [True] * 40)
  assert states.index(LongCtrlState.pid) == STANDSTILL_GO_DWELL_FRAMES - 1
  assert set(states[STANDSTILL_GO_DWELL_FRAMES - 1:]) == {LongCtrlState.pid}


def test_rolling_car_leaves_the_stop_at_once():
  # an interrupted launch: the second go comes at 0.3-0.5 m/s and must not wait
  stop = policy_for(make_cp()).stop_policy()
  assert stop_states(stop, [True] * 3, standstill=False)[0] == LongCtrlState.pid


def test_inactive_resets_the_dwell():
  stop = policy_for(make_cp()).stop_policy()
  stop_states(stop, [True] * (STANDSTILL_GO_DWELL_FRAMES - 1))
  assert stop_states(stop, [True], active=False) == [LongCtrlState.off]
  assert stop.go_frames == 0


def test_longcontrol_holds_the_brake_through_a_standstill_blip():
  # The same 4:36 sequence through openpilot's LongControl. Without the stop policy the blip leaves
  # stopping (+0.41 sent on the car) and the stop ramp restarts from about zero.
  from opendbc.car.structs import CarParams, CarState
  from openpilot.selfdrive.controls.lib.longcontrol import LongControl

  CP = CarParams(brand="mazda", openpilotLongitudinalControl=True, flags=MazdaSafetyFlags.GEN2.value, stopAccel=-0.5)
  CS = CarState(standstill=True, vEgo=0.0, aEgo=0.0)
  plan = [(-0.2, True)] * 100 + [(0.22, False)] * 5 + [(-0.1, True)] * 60

  def drive(lc):
    return [(lc.update(True, CS, target, stop, (-3.5, 2.0)), lc.long_control_state) for target, stop in plan]

  guarded = LongControl(CP)
  assert guarded.extension is not None and guarded.extension.vehicle_stop is not None
  got = drive(guarded)
  assert {state for _, state in got} == {LongCtrlState.stopping}
  assert max(out for out, _ in got[100:]) <= -0.5 + 1e-9

  unguarded = LongControl(CP)
  unguarded.extension.vehicle_stop = None
  base = drive(unguarded)
  assert LongCtrlState.pid in {state for _, state in base[100:105]}
  assert max(out for out, _ in base[100:]) > 0.2


def test_longcontrol_launches_after_the_dwell():
  from opendbc.car.structs import CarParams, CarState
  from openpilot.selfdrive.controls.lib.longcontrol import LongControl

  CP = CarParams(brand="mazda", openpilotLongitudinalControl=True, flags=MazdaSafetyFlags.GEN2.value, stopAccel=-0.5)
  CS = CarState(standstill=True, vEgo=0.0, aEgo=0.0)
  lc = LongControl(CP)
  for _ in range(100):
    lc.update(True, CS, -0.2, True, (-3.5, 2.0))
  states = []
  for _ in range(40):
    lc.update(True, CS, 0.5, False, (-3.5, 2.0))
    states.append(lc.long_control_state)
  assert states.index(LongCtrlState.pid) == STANDSTILL_GO_DWELL_FRAMES - 1
  assert lc.last_output_accel > 0.0
