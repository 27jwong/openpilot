import unittest

import numpy as np

from openpilot.common.test import OpenpilotTestCase
from openpilot.cereal import messaging
from opendbc.car.structs import car
from openpilot.common.params import Params
from openpilot.selfdrive.locationd.longlagd import LongitudinalLagEstimator, fit_split_delay, retrieve_initial_lag, true_accel, \
                                                   BLOCK_NUM_NEEDED
from openpilot.starpilot.longitudinal.longitudinal_delay import LongDelayStatus, brake_threshold
from openpilot.starpilot.schema_cache import put_cache

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


def run_estimator(estimator, t, cmd, v, long_active=True, gas_pressed=False, blend_factor=None):
  cc = car.CarControl.new_message(longActive=long_active)
  cc.actuators.longControlState = "pid"
  cs = car.CarState.new_message(gasPressed=gas_pressed)
  spcs = messaging.new_message('starpilotCarState').starpilotCarState
  if blend_factor is not None:
    spcs.blendedAccInfo.valid = True
    spcs.blendedAccInfo.blendFactor = blend_factor

  for i in range(len(t)):
    cc.actuators.accel = float(cmd[i])
    cs.vEgo = cs.vEgoRaw = float(v[i])
    estimator.handle_log(t[i], "carControl", cc)
    estimator.handle_log(t[i], "carState", cs)
    estimator.handle_log(t[i], "starpilotCarState", spcs)
    estimator.update_points()
    if i % 5 == 0:
      estimator.update_estimate()
  return estimator.get_msg(True).starpilotLongitudinalDelay


def make_estimator(**kwargs):
  CP = car.CarParams.new_message(longitudinalActuatorDelay=0.35, openpilotLongitudinalControl=True)
  return LongitudinalLagEstimator(CP, DT, min_recovery_buffer_sec=0.0, **kwargs)


class TestLonglagd(OpenpilotTestCase):
  def test_fit_recovers_split(self):
    for d_gas, d_brake in [(0.25, 0.5), (0.15, 0.7), (0.3, 0.3)]:
      with self.subTest(d_gas=d_gas, d_brake=d_brake):
        t, cmd = make_command(120, seed=2)
        v = simulate(cmd, d_gas, d_brake, noise=0.01, grade_amp=0.3)
        thr = np.array([brake_threshold(x) for x in v])
        accel, usable = true_accel(t, v, np.ones(len(t), dtype=bool))

        fit = fit_split_delay(cmd, accel, thr, usable, DT)
        self.assertIsNotNone(fit)
        self.assertGreater(fit.r2, 0.9)
        self.assertAlmostEqual(fit.regimes["gas"].delay, d_gas, delta=0.02)
        self.assertAlmostEqual(fit.regimes["brake"].delay, d_brake, delta=0.02)
        self.assertAlmostEqual(fit.regimes["gas"].gain, 1.0, delta=0.05)
        self.assertAlmostEqual(fit.regimes["brake"].gain, 1.0, delta=0.05)

  def test_empty_estimator(self):
    msg = make_estimator().get_msg(True).starpilotLongitudinalDelay
    for regime in (msg.gas, msg.brake):
      self.assertEqual(regime.status, LongDelayStatus.unestimated)
      self.assertAlmostEqual(regime.delay, 0.35, places=5)
      self.assertEqual(regime.validBlocks, 0)
      self.assertEqual(regime.calPerc, 0)

  def test_estimator_learns_split(self):
    t, cmd = make_command(600, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(), t, cmd, v)

    self.assertEqual(msg.gas.status, LongDelayStatus.estimated)
    self.assertEqual(msg.brake.status, LongDelayStatus.estimated)
    self.assertAlmostEqual(msg.gas.delay, 0.25, delta=0.02)
    self.assertAlmostEqual(msg.brake.delay, 0.5, delta=0.02)
    self.assertGreaterEqual(msg.gas.validBlocks, BLOCK_NUM_NEEDED)
    self.assertEqual(msg.gas.calPerc, 100)

  def test_no_braking_leaves_brake_unestimated(self):
    t, cmd = make_command(400, seed=5, offset=0.6, scale=0.5)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(), t, cmd, v)

    self.assertEqual(msg.gas.status, LongDelayStatus.estimated)
    self.assertAlmostEqual(msg.gas.delay, 0.25, delta=0.02)
    self.assertEqual(msg.brake.status, LongDelayStatus.unestimated)
    self.assertEqual(msg.brake.validBlocks, 0)

  def test_rejected_data_is_not_learned(self):
    t, cmd = make_command(300, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    for rejection in ("blended", "gas_pressed", "inactive", "stock_long"):
      with self.subTest(rejection=rejection):
        estimator = make_estimator()
        if rejection == "stock_long":
          estimator.enabled = False
        msg = run_estimator(estimator, t, cmd, v,
                            long_active=rejection != "inactive",
                            gas_pressed=rejection == "gas_pressed",
                            blend_factor=0.5 if rejection == "blended" else None)

        for regime in (msg.gas, msg.brake):
          self.assertEqual(regime.status, LongDelayStatus.unestimated)
          self.assertEqual(regime.validBlocks, 0)

  def test_fully_blended_to_openpilot_is_learned(self):
    t, cmd = make_command(300, seed=3)
    v = simulate(cmd, 0.25, 0.5)
    msg = run_estimator(make_estimator(min_valid_block_count=1), t, cmd, v, blend_factor=1.0)
    self.assertGreater(msg.gas.validBlocks, 0)

  def test_read_saved_params(self):
    params = Params()
    CP = car.CarParams.new_message(carFingerprint="MAZDA_CX_30", longitudinalActuatorDelay=0.35)
    put_cache(params, "CarParamsPrevRoute", CP, block=True)

    msg = messaging.new_message('starpilotLongitudinalDelay')
    msg.starpilotLongitudinalDelay.gas.delayEstimate = 0.27
    msg.starpilotLongitudinalDelay.gas.validBlocks = 7
    msg.starpilotLongitudinalDelay.gas.status = 'estimated'
    msg.starpilotLongitudinalDelay.brake.delayEstimate = 0.9
    msg.starpilotLongitudinalDelay.brake.validBlocks = 6
    msg.starpilotLongitudinalDelay.brake.status = 'invalid'
    params.put("LiveLongitudinalDelay", msg.to_bytes(), block=True)

    initial = retrieve_initial_lag(params, CP)
    self.assertIsNotNone(initial)
    lag, valid_blocks = initial["gas"]
    self.assertAlmostEqual(lag, 0.27, places=5)
    self.assertEqual(valid_blocks, 7)
    # blocks that disagreed start over rather than carrying over
    self.assertNotIn("brake", initial)

    other_CP = car.CarParams.new_message(carFingerprint="MAZDA_3_2019")
    self.assertIsNone(retrieve_initial_lag(params, other_CP))
    self.assertIsNone(params.get("LiveLongitudinalDelay"))


if __name__ == "__main__":
  unittest.main()
