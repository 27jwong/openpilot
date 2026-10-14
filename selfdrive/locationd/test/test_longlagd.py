import numpy as np
import pytest

from cereal import messaging, car
from openpilot.common.params import Params
from openpilot.selfdrive.locationd.longlagd import LongitudinalLagEstimator, SavedRegime, fit_split_delay, initial_from_saved, \
                                                   retrieve_initial_lag, true_accel, BLOCK_NUM, BLOCK_NUM_NEEDED, MAX_LAG_SE
from openpilot.starpilot.common.longitudinal_delay import brake_threshold

DT = 0.05
V0 = 20.0


def make_command(seconds, seed=0, offset=0.0, scale=1.0):
  """A sum of random-phase sinusoids from 0.05 to 0.8Hz, roughly what following traffic asks for."""
  rng = np.random.default_rng(seed)
  t = np.arange(int(seconds / DT)) * DT
  freqs = rng.uniform(0.05, 0.8, 8)
  cmd = sum(0.5 / (1 + 3 * f) * np.sin(2 * np.pi * f * t + rng.uniform(0, 2 * np.pi)) for f in freqs)
  return t, offset + scale * 0.6 * cmd / np.std(cmd)


def simulate(cmd, d_gas, d_brake, noise=0.0, grade_amp=0.0, seed=1):
  """Powertrain answers down to the brake threshold after d_gas, the brakes the rest after d_brake."""
  rng = np.random.default_rng(seed)
  kg, kb = int(round(d_gas / DT)), int(round(d_brake / DT))
  v = np.full(len(cmd), V0)
  accel = np.zeros(len(cmd))
  for i in range(len(cmd)):
    thr = brake_threshold(v[i - 1] if i else V0)
    grade = grade_amp * np.sin(2 * np.pi * 0.01 * i * DT)
    accel[i] = thr + max(cmd[max(i - kg, 0)] - thr, 0.0) + min(cmd[max(i - kb, 0)] - thr, 0.0) + grade
    if i:
      v[i] = v[i - 1] + (accel[i - 1] + accel[i]) / 2 * DT
  return v + rng.normal(0, noise, len(v))


def sparse_braking_command(seconds, seed, event_every=60.0):
  """Mostly small requests with a hard braking event now and then, each followed by the throttle the
  car gives back, so speed stays near V0. About what the CX-30's openpilot-only driving looks like:
  braking past the threshold is a few percent of the time."""
  rng = np.random.default_rng(seed)
  t = np.arange(int(seconds / DT)) * DT
  freqs = rng.uniform(0.05, 0.7, 8)
  cmd = sum(np.sin(2 * np.pi * f * t + rng.uniform(0, 2 * np.pi)) / (1 + 3 * f) for f in freqs)
  cmd = 0.25 * cmd / np.std(cmd)
  start = rng.uniform(5, event_every)
  while start < seconds - 20:
    depth, dur = rng.choice([-1.0, -1.3, -1.7]), rng.uniform(3, 5)
    cmd = cmd + depth * np.clip(np.minimum((t - start) / 1.0, (start + dur - t) / 1.0), 0, 1) * ((t >= start) & (t <= start + dur))
    # give back most of the speed over the next ten seconds (a half sine of 10s has area 10 * 2 / pi per unit height)
    height = 0.9 * -depth * (dur - 1) / (10 * 2 / np.pi)
    cmd = cmd + height * np.sin(np.pi * np.clip((t - start - dur) / 10.0, 0, 1)) * ((t > start + dur) & (t < start + dur + 10))
    start += rng.exponential(event_every) + dur + 10
  return t, cmd


def run_estimator(estimator, t, cmd, v, long_active=True, gas_pressed=False, blend_factor=None):
  cc = car.CarControl.new_message(longActive=long_active)
  cc.actuators.longControlState = "pid"
  cs = car.CarState.new_message(gasPressed=gas_pressed)
  spcc = messaging.new_message('starpilotCarControl').starpilotCarControl
  if blend_factor is not None:
    spcc.blendedAccInfo.valid = True
    spcc.blendedAccInfo.blendFactor = blend_factor

  for i in range(len(t)):
    cc.actuators.accel = float(cmd[i])
    cs.vEgo = cs.vEgoRaw = float(v[i])
    estimator.handle_log(t[i], "carControl", cc)
    estimator.handle_log(t[i], "carState", cs)
    estimator.handle_log(t[i], "starpilotCarControl", spcc)
    estimator.update_points()
    if i % 5 == 0:
      estimator.update_estimate()
  return estimator.get_msg(True).starpilotLongitudinalDelay


def make_estimator(**kwargs):
  CP = car.CarParams(longitudinalActuatorDelay=0.35, openpilotLongitudinalControl=True)
  return LongitudinalLagEstimator(CP, DT, min_recovery_buffer_sec=0.0, **kwargs)


class TestLonglagd:
  @pytest.mark.parametrize("d_gas, d_brake", [(0.25, 0.5), (0.15, 0.7), (0.3, 0.3)])
  def test_fit_recovers_split(self, d_gas, d_brake):
    t, cmd = make_command(120, seed=2)
    v = simulate(cmd, d_gas, d_brake, noise=0.01, grade_amp=0.3)
    thr = np.array([brake_threshold(x) for x in v])
    accel, usable = true_accel(t, v, np.ones(len(t), dtype=bool))

    fit = fit_split_delay(cmd, accel, thr, usable, DT)
    assert fit is not None and fit.r2 > 0.9
    assert fit.regimes["gas"].delay == pytest.approx(d_gas, abs=0.02)
    assert fit.regimes["brake"].delay == pytest.approx(d_brake, abs=0.02)
    assert fit.regimes["gas"].gain == pytest.approx(1.0, abs=0.05)
    assert fit.regimes["brake"].gain == pytest.approx(1.0, abs=0.05)

  def test_empty_estimator(self):
    estimator = make_estimator()
    msg = estimator.get_msg(True).starpilotLongitudinalDelay
    for regime in (msg.gas, msg.brake):
      assert regime.status == 'unestimated'
      assert regime.delay == pytest.approx(0.35)
      assert regime.validBlocks == 0
      assert regime.calPerc == 0

  def test_estimator_learns_split(self):
    # with the brake threshold at -0.75 only the harder braking counts as the brake regime, so the
    # brake side needs about three times the drive it did at the old placeholder table
    t, cmd = make_command(900, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(), t, cmd, v)

    assert msg.gas.status == 'estimated'
    assert msg.brake.status == 'estimated'
    assert msg.gas.delay == pytest.approx(0.25, abs=0.02)
    assert msg.brake.delay == pytest.approx(0.5, abs=0.02)
    assert msg.gas.validBlocks >= BLOCK_NUM_NEEDED and msg.gas.calPerc == 100

  def test_no_braking_leaves_brake_unestimated(self):
    t, cmd = make_command(400, seed=5, offset=0.6, scale=0.5)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(), t, cmd, v)

    assert msg.gas.status == 'estimated'
    assert msg.gas.delay == pytest.approx(0.25, abs=0.02)
    assert msg.brake.status == 'unestimated'
    assert msg.brake.validBlocks == 0

  @pytest.mark.parametrize("rejection", ["blended", "gas_pressed", "inactive", "stock_long"])
  def test_rejected_data_is_not_learned(self, rejection):
    t, cmd = make_command(300, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    estimator = make_estimator()
    if rejection == "stock_long":
      estimator.enabled = False
    msg = run_estimator(estimator, t, cmd, v,
                        long_active=rejection != "inactive",
                        gas_pressed=rejection == "gas_pressed",
                        blend_factor=0.5 if rejection == "blended" else None)

    for regime in (msg.gas, msg.brake):
      assert regime.status == 'unestimated'
      assert regime.validBlocks == 0

  def test_fully_blended_to_openpilot_is_learned(self):
    t, cmd = make_command(300, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(min_valid_block_count=1), t, cmd, v, blend_factor=1.0)
    assert msg.gas.validBlocks > 0

  def test_read_saved_params(self):
    params = Params()
    CP = car.CarParams(carFingerprint="MAZDA_CX_30", longitudinalActuatorDelay=0.35)
    params.put("CarParamsPrevRoute", CP.to_bytes())

    msg = messaging.new_message('starpilotLongitudinalDelay')
    msg.starpilotLongitudinalDelay.gas.delayEstimate = 0.27
    msg.starpilotLongitudinalDelay.gas.validBlocks = 7
    msg.starpilotLongitudinalDelay.gas.status = 'estimated'
    msg.starpilotLongitudinalDelay.brake.delayEstimate = 0.9
    msg.starpilotLongitudinalDelay.brake.validBlocks = 6
    msg.starpilotLongitudinalDelay.brake.status = 'invalid'
    params.put("LiveLongitudinalDelay", msg.to_bytes())

    initial = retrieve_initial_lag(params, CP)
    assert initial is not None
    # a save from before the blocks were kept still restarts from its average
    assert initial["gas"].lag == pytest.approx(0.27) and initial["gas"].valid_blocks == 7 and initial["gas"].values is None
    # blocks that disagreed start over rather than carrying over
    assert "brake" not in initial

    other_CP = car.CarParams(carFingerprint="MAZDA_3_2019")
    assert retrieve_initial_lag(params, other_CP) is None
    assert params.get("LiveLongitudinalDelay") is None

  def test_state_survives_a_restart(self):
    t, cmd = make_command(500, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    first = make_estimator()
    run_estimator(first, t, cmd, v)
    saved = first.get_msg(True, persist=True).starpilotLongitudinalDelay
    # the live message leaves the blocks out; the saved one carries them, and the filling block
    assert len(first.get_msg(True).starpilotLongitudinalDelay.gas.points) == 0
    assert len(saved.gas.points) == BLOCK_NUM and saved.brake.blockFill > 0

    second = make_estimator()
    second.reset(initial_from_saved(saved))
    restored = second.get_msg(True, persist=True).starpilotLongitudinalDelay
    for name in ("gas", "brake"):
      a, b = getattr(saved, name), getattr(restored, name)
      assert (b.status, b.validBlocks, b.blockIndex, b.blockFill) == (a.status, a.validBlocks, a.blockIndex, a.blockFill)
      assert b.delay == pytest.approx(a.delay) and b.delayEstimate == pytest.approx(a.delayEstimate)
      assert list(b.points) == pytest.approx(list(a.points))

    # and it carries on from there: the next drive's estimates land in the same blocks
    t2, cmd2 = make_command(200, seed=4)
    run_estimator(second, t2, cmd2, simulate(cmd2, 0.25, 0.5))
    assert second.get_msg(True).starpilotLongitudinalDelay.gas.validBlocks > saved.gas.validBlocks

  def test_unusable_saved_blocks_fall_back_to_the_average(self):
    t, cmd = make_command(500, seed=3)
    first = make_estimator()
    run_estimator(first, t, cmd, simulate(cmd, 0.25, 0.5))
    saved = initial_from_saved(first.get_msg(True, persist=True).starpilotLongitudinalDelay)

    saved["gas"].values[3] = float('nan')
    saved["brake"].values = saved["brake"].values[:-1]
    estimator = make_estimator()
    estimator.reset(saved)
    msg = estimator.get_msg(True, persist=True).starpilotLongitudinalDelay
    for name in ("gas", "brake"):
      regime = getattr(msg, name)
      assert regime.validBlocks == saved[name].valid_blocks and regime.blockFill == 0
      assert list(regime.points) == pytest.approx([saved[name].lag] * BLOCK_NUM)

  @pytest.mark.parametrize("blocks, expected", [
    ([0.08, 0.32] * 3, 'unestimated'),   # six blocks 0.12s apart: mean good to 0.049s, not yet
    ([0.08, 0.32] * 9, 'estimated'),     # eighteen of them: good to 0.028s
    ([0.0, 0.6] * 9, 'invalid'),         # scatter of 0.3s is no single delay
    ([0.2] * 4, 'unestimated'),          # too few blocks however steady
    ([0.2, 0.21] * 3, 'estimated'),
  ])
  def test_delay_is_trusted_once_its_mean_is_pinned_down(self, blocks, expected):
    estimator = make_estimator()
    saved = np.full(BLOCK_NUM, 0.2)
    saved[:len(blocks)] = blocks
    estimator.reset({"brake": SavedRegime(float(np.mean(blocks)), len(blocks), saved.tolist(), len(blocks), 0)})
    regime = estimator.get_msg(True).starpilotLongitudinalDelay.brake
    assert regime.status == expected
    assert (regime.calPerc == 100) == (expected == 'estimated')
    if expected == 'estimated':
      assert regime.delay == pytest.approx(np.mean(blocks), abs=1e-6)
      assert np.std(blocks) / np.sqrt(len(blocks)) <= MAX_LAG_SE

  def test_brake_is_learned_across_short_drives(self):
    # what the CX-30 gives it: a few minutes engaged per drive, a hard braking event every minute or so.
    # Before partial blocks were kept, 50s of braking had to land in one drive; now it adds up.
    saved, statuses = None, []
    for drive in range(10):
      t, cmd = sparse_braking_command(300, seed=drive)
      v = simulate(cmd, 0.4, 0.2, noise=0.005)
      assert v.min() > 9.0, "the scenario is meant to hold speed"
      estimator = make_estimator()
      if saved is not None:
        estimator.reset(initial_from_saved(saved))
      msg = run_estimator(estimator, t, cmd, v)
      saved = estimator.get_msg(True, persist=True).starpilotLongitudinalDelay
      statuses.append((msg.gas.status, msg.brake.status))

    assert statuses[-1] == ('estimated', 'estimated'), statuses
    assert msg.gas.delay == pytest.approx(0.4, abs=0.04)
    assert msg.brake.delay == pytest.approx(0.2, abs=0.05)
    # not on the first drive, which has too little braking to settle anything
    assert statuses[0][1] != 'estimated'

  def test_noisy_windows_are_learned_without_bias(self):
    # wheel-speed noise puts the windows' r2 in the 0.5-0.7 band the gate now lets through
    t, cmd = make_command(1800, seed=4)
    v = simulate(cmd, 0.4, 0.2, noise=0.15, grade_amp=0.2)
    estimator = make_estimator()
    msg = run_estimator(estimator, t, cmd, v)
    assert 0.5 <= estimator.last_fit.r2 < 0.7, estimator.last_fit.r2
    assert msg.gas.status == msg.brake.status == 'estimated'
    assert msg.gas.delay == pytest.approx(0.4, abs=0.04)
    assert msg.brake.delay == pytest.approx(0.2, abs=0.05)

  def test_outcomes_say_why_estimates_were_taken_or_not(self):
    t, cmd = make_command(300, seed=3)
    estimator = make_estimator()
    run_estimator(estimator, t, cmd, simulate(cmd, 0.25, 0.5))
    # a clean two-path drive: every regime is either learning or waiting for new data
    assert set(estimator.last_outcome.values()) <= {"accepted", "no new data"}

    # wheel speed that has nothing to do with the command can't be fitted at all
    rng = np.random.default_rng(0)
    estimator = make_estimator()
    run_estimator(estimator, t, cmd, V0 + rng.normal(0, 0.05, len(t)).cumsum() * 0.05)
    assert set(estimator.last_outcome.values()) == {"fit r2"}
    assert estimator.last_fit is not None and estimator.last_fit.r2 < estimator.min_r2
