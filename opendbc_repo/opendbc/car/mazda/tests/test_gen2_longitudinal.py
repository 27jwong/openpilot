from types import SimpleNamespace

import pytest

import numpy as np

from opendbc.car.mazda.gen2_longitudinal import (KI, KP, MAX_BRAKE_OVERSHOOT, MAX_GAS_OVERSHOOT, ERROR_FILTER_RC,
                                                 LOW_SPEED_BRAKE_OVERSHOOT, RESPONSE_WARMUP_FRAMES, SHORTFALL_COMPENSATION_MAX,
                                                 STANDSTILL_GO_DWELL_FRAMES, LongCtrlState, brake_overshoot,
                                                 low_speed_shortfall, policy_for)
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.pid import PIDController
from openpilot.common.realtime import DT_CTRL


def make_cp(brand="mazda", long=True, flags=MazdaSafetyFlags.GEN2.value):
  return SimpleNamespace(brand=brand, openpilotLongitudinalControl=long, flags=flags)


def run(policy, pid, targets, a_ego, speed=10.0):
  """LongControl's pid branch, as extension.prepare_pid / shape_output drive it."""
  outs = []
  last = 0.0
  for target, ego in zip(targets, a_ego, strict=True):
    error = target - ego
    ff = policy.feedforward(target, speed, last)
    freeze = policy.prepare_pid(pid, target, error, speed, last, (-3.5, 2.0))
    out = pid.update(error, speed=speed, feedforward=ff, freeze_integrator=freeze)
    last = float(np.clip(policy.shape_output(float(out), target, error, speed), -3.5, 2.0))
    outs.append(last)
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
  # the raw error goes through: no filter correction, and the filter restarts from it
  assert policy.error_delta == 0.0 and policy.error_filter.x == 3.0
  # the unfiltered P term (1.0 + kp * 3.0) is then held to the gas bound
  assert 1.0 + float(np.interp(10.0, *KP)) * 3.0 > 1.0 + MAX_GAS_OVERSHOOT
  assert np.allclose(got, 1.0 + MAX_GAS_OVERSHOOT)


def test_brake_overshoot_is_bounded_and_integrator_follows():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  # planner wants -1.0 but the car is not decelerating yet: feedback would out-brake it
  got = run(policy, pid, np.full(300, -1.0), np.full(300, 0.0))
  # through the car's dead time and lag that is only the PI's lead, and the bound holds it
  lag = RESPONSE_WARMUP_FRAMES
  assert got[:lag].min() >= -1.0 - MAX_BRAKE_OVERSHOOT - 1e-9
  assert np.isclose(got[lag - 1], -1.0 - MAX_BRAKE_OVERSHOOT)
  # a car still not braking after that is short of its own response, and the command makes it up, to a limit
  assert got.min() >= -1.0 - MAX_BRAKE_OVERSHOOT - SHORTFALL_COMPENSATION_MAX - 1e-9
  assert np.isclose(got[-1], -1.0 - MAX_BRAKE_OVERSHOOT - SHORTFALL_COMPENSATION_MAX)
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
  # At crawl speed the floor also carries the low-speed braking shortfall, so it is ~2x steeper in the target
  # there: this 0.02 target step moves it 0.022. Still no snap (the old one was 0.56).
  assert np.abs(np.diff(got)).max() < 0.03


def test_gas_overshoot_is_bounded_and_integrator_follows():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=4.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  # planner wants +1.5 but the car has not picked up yet: feedback would push far past it
  got = run(policy, pid, np.full(300, 1.5), np.full(300, 0.3))
  assert got.max() <= 1.5 + MAX_GAS_OVERSHOOT + 1e-9
  assert np.isclose(got[-1], 1.5 + MAX_GAS_OVERSHOOT)
  # back-calculated: the integrator holds only what was actually sent
  assert pid.i <= MAX_GAS_OVERSHOOT + 1e-6


def test_wound_up_gas_lets_go_when_the_target_drops():
  # CX-30 route 00000028 8:57: chill stepped the target to 1.9 for a second, the car lagged at ~0.7, feedback
  # sent 3.3 and the integrator kept pulling after the target came back to 0.8
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=4.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  targets = np.concatenate([np.full(100, 1.9), np.full(50, 0.8)])
  a_ego = np.concatenate([np.full(100, 0.7), np.full(50, 1.9)])
  got = run(policy, pid, targets, a_ego, speed=11.0)
  assert got[:100].max() <= 1.9 + MAX_GAS_OVERSHOOT + 1e-9
  assert got[100:].max() <= 0.8 + MAX_GAS_OVERSHOOT + 1e-9


def test_gas_ceiling_holds_for_a_target_asking_for_brake():
  # route 00000028 10:09: the car braked harder than the -0.40 asked and feedback sent +0.38 of gas
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=4.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  got = run(policy, pid, np.full(200, -0.4), np.full(200, -1.5), speed=9.0)
  assert got.max() <= MAX_GAS_OVERSHOOT + 1e-9


def test_gas_ceiling_is_continuous_where_the_target_crosses_zero():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=4.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  targets = np.concatenate([np.full(100, -0.01), np.full(20, 0.01)])
  got = run(policy, pid, targets, np.full(120, -0.8), speed=0.8)
  assert np.abs(np.diff(got)).max() < 0.02


def test_ordinary_tracking_never_meets_the_gas_ceiling():
  # a car that follows the target with the usual noise stays well inside both bounds
  rng = np.random.default_rng(1)
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=4.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  targets = 0.6 * np.sin(np.linspace(0, 6 * np.pi, 1500))
  got = run(policy, pid, targets, targets + rng.normal(0, 0.05, 1500), speed=15.0)
  assert (got - targets).max() < MAX_GAS_OVERSHOOT


# Measured CX-30 braking shortfall (median aEgo - steady command), routes 25-29.
SHORTFALL_V = [0.6, 1.5, 2.75, 4.25, 6.5, 10.0]
SHORTFALL_U = [-2.0, -1.5, -1.1, -0.8, -0.5, -0.3, 0.0]
SHORTFALL = [[0.30, 0.30, 0.28, 0.28, 0.20, 0.13, 0.0],
             [0.45, 0.42, 0.17, 0.26, 0.05, 0.03, 0.0],
             [0.31, 0.40, 0.18, 0.20, 0.00, -0.06, 0.0],
             [0.25, 0.28, 0.10, 0.04, 0.04, -0.02, 0.0],
             [0.09, 0.10, 0.03, -0.06, -0.08, -0.08, 0.0],
             [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]]


def cx30_braking(speed, command):
  if command >= 0:
    return command
  row = [np.interp(command, SHORTFALL_U, r) for r in SHORTFALL]
  return command + float(np.interp(speed, SHORTFALL_V, row))


def closed_loop(target, speed, seconds=3.0, policy=None):
  """The policy and PI against a 0.15 s dead time + 0.4 s lag car with the measured braking shortfall."""
  policy = policy or policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  a, sent = 0.0, [0.0] * 15
  for _ in range(int(seconds / DT_CTRL)):
    error = target - a
    ff = policy.feedforward(target, speed, sent[-1])
    freeze = policy.prepare_pid(pid, target, error, speed, sent[-1], (-3.5, 2.0))
    out = policy.shape_output(float(pid.update(error, speed=speed, feedforward=ff, freeze_integrator=freeze)), target, error, speed)
    out = float(np.clip(out, -3.5, 2.0))
    sent.append(out)
    a += DT_CTRL / 0.4 * (cx30_braking(speed, sent[-16]) - a)
  return a, out


def test_shortfall_is_zero_unless_braking_at_low_speed():
  for speed in (0.0, 0.5, 2.0, 4.0):
    assert low_speed_shortfall(0.0, speed) == 0.0 and low_speed_shortfall(0.5, speed) == 0.0
    assert brake_overshoot(0.0, speed) == MAX_BRAKE_OVERSHOOT and brake_overshoot(0.5, speed) == MAX_BRAKE_OVERSHOOT
  for target in (-0.3, -1.0, -2.0):
    assert low_speed_shortfall(target, 6.0) == 0.0 and brake_overshoot(target, 7.0) == MAX_BRAKE_OVERSHOOT
  # grows with the braking asked for, capped, and at full width below 3 m/s
  assert 0 < low_speed_shortfall(-0.3, 2.5) < low_speed_shortfall(-1.0, 2.5) <= low_speed_shortfall(-2.0, 2.5) <= 0.3
  assert brake_overshoot(-1.0, 2.0) == LOW_SPEED_BRAKE_OVERSHOOT


@pytest.mark.parametrize('speed, target', ((0.6, -0.35), (0.6, -0.5), (1.5, -1.5), (2.75, -1.1), (4.25, -1.5)))
def test_low_speed_braking_reaches_the_request(speed, target):
  # route 00000029 5:48-5:53: the plain bound left the car 0.4-0.6 m/s^2 short and then creeping at 1 mph
  a, _ = closed_loop(target, speed)
  assert abs(a - target) < 0.05

  class Plain:  # the policy before the low-speed terms: plain feedforward and the fixed 0.10 bound
    def __init__(self):
      self.p = policy_for(make_cp())
      self.p.response.compensation = lambda target, speed: 0.0  # nor the measured shortfall
    def feedforward(self, t, v, last):
      return t
    def prepare_pid(self, *args, **kwargs):
      return self.p.prepare_pid(*args, **kwargs)
    def shape_output(self, out, t, e, v):
      out = self.p.shape_output(out, t, e, 10.0)  # 10 m/s: no low-speed terms
      return out
  a_plain, _ = closed_loop(target, speed, policy=Plain())
  assert a_plain - target > 0.07


def test_highway_braking_is_unchanged():
  # above ~6 m/s the car brakes as asked, so neither term may add braking there
  policy = policy_for(make_cp())
  assert policy.feedforward(-1.0, 15.0, 0.0) == -1.0
  a, out = closed_loop(-1.0, 15.0)
  assert out >= -1.0 - MAX_BRAKE_OVERSHOOT - 1e-9


def trace(targets, speed, deliver, policy=None, noise=0.0, seed=0):
  """closed_loop with a target per frame, a delivery function and aEgo noise; returns (car accel, sent)."""
  policy = policy or policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  rng = np.random.default_rng(seed)
  a, sent, accels = 0.0, [0.0] * 15, []
  for target in targets:
    a_ego = a + rng.normal(0.0, noise) if noise else a
    error = target - a_ego
    ff = policy.feedforward(target, speed, sent[-1])
    freeze = policy.prepare_pid(pid, target, error, speed, sent[-1], (-3.5, 2.0))
    out = policy.shape_output(float(pid.update(error, speed=speed, feedforward=ff, freeze_integrator=freeze)), target, error, speed)
    sent.append(float(np.clip(out, -3.5, 2.0)))
    a += DT_CTRL / 0.4 * (deliver(sent[-16]) - a)
    accels.append(a)
  return np.array(accels), np.array(sent[15:])


def test_car_following_its_response_keeps_the_plain_bound():
  # a hard brake ramp (0 to -2.5 in 1 s) is where the PI leads the car's lag the most; a car that answers the
  # way it was identified has no shortfall to make up, so the plain 0.10 bound holds throughout
  targets = np.concatenate([np.linspace(0.0, -2.5, 100), np.full(250, -2.5)])
  policy = policy_for(make_cp())
  _, sent = trace(targets, 15.0, lambda u: u, policy=policy, noise=0.05)
  assert (sent >= targets - MAX_BRAKE_OVERSHOOT - 1e-9).all()
  assert policy.response.compensation(-2.5, 15.0) == 0.0


def test_car_short_of_its_response_is_made_up():
  # route 0000002b 2:37.3-2:39.0, 24 mph: cresting a hill the car delivered 0.5-0.6 m/s^2 less braking than it
  # was sent, and the plain bound held the command at target - 0.10, so it stayed ~0.45 short for 1.5 s
  targets = np.full(300, -1.1)
  short = lambda u: u + 0.55 if u < 0 else u  # noqa: E731
  plain = policy_for(make_cp())
  plain.response.compensation = lambda target, speed: 0.0
  a_plain, _ = trace(targets, 10.9, short, policy=plain)
  a, sent = trace(targets, 10.9, short)
  assert a_plain[-1] - targets[-1] > 0.4
  assert abs(a[200:] - targets[200:]).max() < 0.15  # within 0.15 of the plan 2 s in (the deadband is left to the PI)
  assert sent.min() >= -1.1 - MAX_BRAKE_OVERSHOOT - SHORTFALL_COMPENSATION_MAX - 1e-9


def test_compensation_vanishes_at_a_zero_target():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  run(policy, pid, np.full(300, -1.0), np.full(300, 0.0))  # the car never braked: a large shortfall
  room = policy.response.compensation
  assert room(-1.0, 10.0) == SHORTFALL_COMPENSATION_MAX
  assert room(0.0, 10.0) == 0.0 and room(0.5, 10.0) == 0.0
  assert room(-0.001, 10.0) < 0.001  # scales with the braking asked for, so the command and floor stay continuous
  # and none at stop-and-go speeds, where the mapped low-speed terms own the braking
  assert room(-1.0, 4.0) == 0.0 and 0.0 < room(-1.0, 5.0) < SHORTFALL_COMPENSATION_MAX


def test_frames_the_policy_did_not_shape_restart_the_response():
  policy = policy_for(make_cp())
  pid = PIDController(KP, KI, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  run(policy, pid, np.full(300, -1.0), np.full(300, 0.0))
  assert policy.response.compensation(-1.0, 10.0) > 0.0
  # a stop or a launch in between: LongControl sent something this policy never returned
  policy.prepare_pid(pid, -1.0, -1.0, 10.0, -0.3, (-3.5, 2.0))
  assert policy.response.frames == 0 and policy.response.compensation(-1.0, 10.0) == 0.0
  run(policy, pid, np.full(300, -1.0), np.full(300, 0.0))
  policy.reset()
  assert policy.response.frames == 0 and policy.expected_delivery is None


def cruise_amplification(kp, ki, freq, speed=20.0):
  """Felt accel over an injected disturbance, with the planner's cruise speed loop (accel = speed error per
  second) closed around the PI and a 0.15 s + 0.4 s car, the way plannerd and controlsd run it at cruise."""
  pid = PIDController(kp, ki, pos_limit=2.0, neg_limit=-3.5, rate=1 / DT_CTRL)
  a = v = 0.0; sent = [0.0] * 15; target = 0.0; felt = []; dist = []
  for k in range(int(30 / DT_CTRL)):
    t = k * DT_CTRL
    d = 0.05 * np.sin(2 * np.pi * freq * t)
    if k % 5 == 0:  # 20 Hz planner
      target = float(np.clip(-v, -0.5, 0.6))
    out = pid.update(target - (a + d), speed=speed, feedforward=target)
    sent.append(out)
    a += DT_CTRL / 0.4 * (sent[-16] - a)
    v += DT_CTRL * (a + d)
    if t > 15:
      felt.append(a + d); dist.append(d)
  return np.std(felt) / np.std(dist)


def test_cruise_tune_rocks_less_than_the_old_one():
  # route 00000029: a ~0.4-1 Hz rocking at steady no-lead cruise, the two speed loops amplifying the car's own
  # wobble; the at-speed gains have to take the resonance down without changing the low-speed tune
  old_kp, old_ki = ([0., 12., 30.], [0.2, 0.5, 0.4]), ([0., 35.], [1.0, 1.0])
  old = max(cruise_amplification(old_kp, old_ki, f) for f in (0.3, 0.4, 0.5, 0.7, 1.0))
  new = max(cruise_amplification(KP, KI, f) for f in (0.3, 0.4, 0.5, 0.7, 1.0))
  assert old > 1.0  # the old tune amplified it
  assert new < old - 0.05
  for v in (0.0, 4.0, 8.0):
    assert np.interp(v, *KI) == 1.0


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
