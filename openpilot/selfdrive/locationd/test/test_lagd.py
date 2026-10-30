import random
import numpy as np
import pytest
import time
import unittest
from functools import cache

from openpilot.common.test import OpenpilotTestCase
from openpilot.cereal import messaging, log
from opendbc.car.structs import car
from openpilot.selfdrive.locationd.lagd import LateralLagEstimator, retrieve_initial_lag, retrieve_speed_bins, \
                                               masked_normalized_cross_correlation, speed_bin_idx, \
                                               BLOCK_NUM_NEEDED, BLOCK_SIZE, MIN_OKAY_WINDOW_SEC, VERSION, MIN_LAG, MAX_LAG, \
                                               SPEED_BIN_SPEEDS, SPEED_BIN_MIN_VEGO, SPEED_BIN_ENTRY_BUFFER_SEC, SPEED_BINS_PARAM
from openpilot.common.params import Params
from openpilot.starpilot.schema_cache import put_cache
from openpilot.common.hardware import PC

MAX_ERR_FRAMES = 1
DT = 0.05
LAGD_MIN_LAG_FRAMES, LAGD_MAX_LAG_FRAMES = int(round(MIN_LAG / DT)), int(round(MAX_LAG / DT))


@cache
def get_test_car_params():
  return car.CarParams.new_message(carFingerprint="cache-test-car", steerRatio=15.0)


def process_messages(estimator, lag_frames, n_frames, vego=25.0, rejection_threshold=0.0, t_offset=0.0):
  for i in range(n_frames):
    t = t_offset + i * estimator.dt
    desired_la = np.cos(10 * t) * 0.3
    actual_la = np.cos(10 * (t - lag_frames * estimator.dt)) * 0.3

    # if sample is masked out, set it to desired value (no lag)
    rejected = random.uniform(0, 1) < rejection_threshold
    if rejected:
      actual_la = desired_la

    desired_cuvature = float(desired_la / (vego ** 2))
    actual_yr = float(actual_la / vego)
    msgs = [
      (t, "carControl", car.CarControl(latActive=not rejected)),
      (t, "carState", car.CarState(vEgo=vego, steeringPressed=False)),
      (t, "controlsState", log.ControlsState(desiredCurvature=desired_cuvature)),
      (t, "deviceMotion", log.DeviceMotion(angularVelocityDevice=log.DeviceMotion.XYZMeasurement(z=actual_yr, valid=True),
                                   posenetOK=True, inputsOK=True)),
      (t, "extrinsicsCalibration", log.ExtrinsicsCalibration(rpyCalib=[0, 0, 0], calStatus=log.ExtrinsicsCalibration.Status.calibrated)),
    ]
    for t, w, m in msgs:
      estimator.handle_log(t, w, m)
    estimator.update_points()
    estimator.update_estimate()


class TestLagd(OpenpilotTestCase):
  def test_unqualified_saved_params_are_not_loaded(self):
    params = Params()
    CP = get_test_car_params()
    msg = messaging.new_message('lateralDelay')
    msg.lateralDelay.lateralDelayEstimate = 0.15
    msg.lateralDelay.validBlocks = 5
    msg.lateralDelay.version = VERSION
    raw = msg.to_bytes()
    params.put("LiveDelay", raw, block=True)
    put_cache(params, "CarParamsPrevRoute", CP, block=True)

    self.assertIsNone(retrieve_initial_lag(params, CP))
    self.assertEqual(params.get("LiveDelay"), raw)

  def test_read_saved_params(self):
    params = Params()

    CP = get_test_car_params()

    msg = messaging.new_message('lateralDelay')
    msg.lateralDelay.lateralDelayEstimate = random.random()
    msg.lateralDelay.validBlocks = random.randint(1, 10)
    msg.lateralDelay.version = VERSION
    put_cache(params, "LiveDelay", msg, block=True)
    put_cache(params, "CarParamsPrevRoute", CP, block=True)

    saved_lag_params = retrieve_initial_lag(params, CP)
    assert saved_lag_params is not None

    lag, valid_blocks = saved_lag_params
    assert lag == msg.lateralDelay.lateralDelayEstimate
    assert valid_blocks == msg.lateralDelay.validBlocks

  def test_read_invalid_saved_params(self, subtests):
    params = Params()

    CP = get_test_car_params()

    for msg_dict in [{'version': 0}, {'status': 'invalid'}, {'validBlocks': 100}]:
      with subtests.test(msg=f"lateralDelay={msg_dict}"):
        msg = messaging.new_message('lateralDelay')
        msg.lateralDelay = msg_dict
        put_cache(params, "LiveDelay", msg, block=True)
        put_cache(params, "CarParamsPrevRoute", CP, block=True)
        assert retrieve_initial_lag(params, CP) is None

  def test_ncc(self):
    rng = np.random.default_rng()
    lag_frames = random.randint(1, 19)

    desired_sig = np.sin(np.arange(0.0, 10.0, 0.1))
    actual_sig = np.sin(np.arange(0.0, 10.0, 0.1) - lag_frames * 0.1)
    mask = np.ones(len(desired_sig), dtype=bool)

    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr) == lag_frames

    # add some noise
    desired_sig += rng.normal(0, 0.05, len(desired_sig))
    actual_sig += rng.normal(0, 0.05, len(actual_sig))
    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr)  in range(lag_frames - MAX_ERR_FRAMES, lag_frames + MAX_ERR_FRAMES + 1)

    # mask out 40% of the values, and make them noise
    mask = rng.choice([True, False], size=len(desired_sig), p=[0.6, 0.4])
    desired_sig[~mask] = rng.normal(0, 1, size=np.sum(~mask))
    actual_sig[~mask] = rng.normal(0, 1, size=np.sum(~mask))
    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr) in range(lag_frames - MAX_ERR_FRAMES, lag_frames + MAX_ERR_FRAMES + 1)

  def test_empty_estimator(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(mocked_CP, DT)
    msg = estimator.get_msg(True)
    assert msg.lateralDelay.status == 'unestimated'
    assert np.allclose(msg.lateralDelay.lateralDelay, estimator.initial_lag)
    assert np.allclose(msg.lateralDelay.lateralDelayEstimate, estimator.initial_lag)
    assert msg.lateralDelay.validBlocks == 0
    assert msg.lateralDelay.calPerc == 0

  def test_estimator_basics(self, subtests):
    for lag_frames in range(LAGD_MIN_LAG_FRAMES, LAGD_MAX_LAG_FRAMES - 1):
      with subtests.test(msg=f"lag_frames={lag_frames}"):
        mocked_CP = car.CarParams(steerActuatorDelay=0.5)
        estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)
        process_messages(estimator, lag_frames, int(MIN_OKAY_WINDOW_SEC / DT) + BLOCK_NUM_NEEDED * BLOCK_SIZE)
        msg = estimator.get_msg(True)
        assert msg.lateralDelay.status == 'estimated'
        assert np.allclose(msg.lateralDelay.lateralDelay, lag_frames * DT, atol=0.01)
        assert np.allclose(msg.lateralDelay.lateralDelayEstimate, lag_frames * DT, atol=0.01)
        assert np.allclose(msg.lateralDelay.lateralDelayEstimateStd, 0.0, atol=0.01)
        assert msg.lateralDelay.validBlocks == BLOCK_NUM_NEEDED
        assert msg.lateralDelay.calPerc == 100

  def test_estimator_masking(self):
    mocked_CP, lag_frames = car.CarParams(steerActuatorDelay=0.5), random.randint(LAGD_MIN_LAG_FRAMES, LAGD_MAX_LAG_FRAMES - 1)
    estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0, min_valid_block_count=1)
    process_messages(estimator, lag_frames, (int(MIN_OKAY_WINDOW_SEC / DT) + BLOCK_SIZE) * 2, rejection_threshold=0.4)
    msg = estimator.get_msg(True)
    assert np.allclose(msg.lateralDelay.lateralDelayEstimate, lag_frames * DT, atol=0.01)
    assert np.allclose(msg.lateralDelay.lateralDelayEstimateStd, 0.0, atol=0.01)
    assert msg.lateralDelay.calPerc == 100

  def test_speed_bin_hysteresis(self):
    assert speed_bin_idx(SPEED_BIN_MIN_VEGO - 0.1, None) is None
    assert speed_bin_idx(SPEED_BIN_MIN_VEGO + 0.1, None) == 0
    assert speed_bin_idx(40.0, None) == len(SPEED_BIN_SPEEDS) - 1

    # jitter around the 19.0 m/s edge stays in whichever bin it started in
    for start_v, expected_bin in ((19.2, 2), (18.8, 1)):
      speed_bin = speed_bin_idx(start_v, None)
      for v in (19.3, 18.7, 19.2, 18.8, 19.4, 18.6):
        speed_bin = speed_bin_idx(v, speed_bin)
        assert speed_bin == expected_bin

    assert speed_bin_idx(18.4, 2) == 1
    assert speed_bin_idx(19.6, 1) == 2

  def test_speed_bin_interpolation(self):
    CP = car.CarParams(steerActuatorDelay=0.1)
    estimator = LateralLagEstimator(CP, DT)
    estimator.reset(0.3, BLOCK_NUM_NEEDED, [None, (0.2, BLOCK_NUM_NEEDED), (0.4, 2), None, None, None])

    def lateral_delay_at(v_ego):
      estimator.handle_log(0.0, "carState", car.CarState(vEgo=v_ego))
      return estimator.get_msg(True).lateralDelay.lateralDelay

    partial_bin_lag = 2 / BLOCK_NUM_NEEDED * 0.4 + (1 - 2 / BLOCK_NUM_NEEDED) * 0.3
    assert lateral_delay_at(SPEED_BIN_SPEEDS[0]) == pytest.approx(0.3)
    assert lateral_delay_at(SPEED_BIN_SPEEDS[1]) == pytest.approx(0.2)
    assert lateral_delay_at(SPEED_BIN_SPEEDS[2]) == pytest.approx(partial_bin_lag)
    assert lateral_delay_at((SPEED_BIN_SPEEDS[0] + SPEED_BIN_SPEEDS[1]) / 2) == pytest.approx(0.25)
    assert lateral_delay_at((SPEED_BIN_SPEEDS[1] + SPEED_BIN_SPEEDS[2]) / 2) == pytest.approx((0.2 + partial_bin_lag) / 2)
    assert lateral_delay_at(0.0) == pytest.approx(0.3)
    assert lateral_delay_at(50.0) == pytest.approx(0.3)

    bins = estimator.get_speed_bins_state(CP)['bins']
    assert [b['speed'] for b in bins] == pytest.approx(SPEED_BIN_SPEEDS)
    assert [b['lateralDelay'] for b in bins] == pytest.approx([0.3, 0.2, partial_bin_lag, 0.3, 0.3, 0.3])
    assert [b['validBlocks'] for b in bins] == [0, BLOCK_NUM_NEEDED, 2, 0, 0, 0]
    assert bins[1]['status'] == 'estimated'
    assert bins[2]['status'] == 'unestimated'

  def test_speed_bins_learn_separately(self):
    CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)

    # both speeds are below the all-speed learner's MIN_VEGO, so only the bins learn
    n_frames = int((MIN_OKAY_WINDOW_SEC + 2 * SPEED_BIN_ENTRY_BUFFER_SEC) / DT) + BLOCK_NUM_NEEDED * BLOCK_SIZE
    process_messages(estimator, 4, n_frames, vego=SPEED_BIN_SPEEDS[1])
    process_messages(estimator, 6, n_frames, vego=SPEED_BIN_SPEEDS[2], t_offset=n_frames * DT)

    msg = estimator.get_msg(True)
    assert msg.lateralDelay.status == 'unestimated'
    bins = estimator.get_speed_bins_state(CP)['bins']
    assert bins[1]['status'] == 'estimated' and bins[2]['status'] == 'estimated'
    assert bins[1]['lateralDelayEstimate'] == pytest.approx(4 * DT, abs=0.01)
    assert bins[2]['lateralDelayEstimate'] == pytest.approx(6 * DT, abs=0.01)

    # bins with no data of their own follow the all-speed value
    for i in (0, 3, 4, 5):
      assert bins[i]['validBlocks'] == 0
      assert bins[i]['lateralDelay'] == pytest.approx(estimator.initial_lag)

    estimator.handle_log(0.0, "carState", car.CarState(vEgo=(SPEED_BIN_SPEEDS[1] + SPEED_BIN_SPEEDS[2]) / 2))
    assert estimator.get_msg(True).lateralDelay.lateralDelay == pytest.approx(5 * DT, abs=0.01)

  def test_speed_bins_alternating(self):
    # short alternating visits: each bin's window joins its visits end to end, and the entry buffer
    # keeps the correlation from pairing samples across those seams
    CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)

    visit_frames = int(2.0 / DT)
    for i in range(60):
      lag_frames, vego = ((4, SPEED_BIN_SPEEDS[1]), (6, SPEED_BIN_SPEEDS[2]))[i % 2]
      process_messages(estimator, lag_frames, visit_frames, vego=vego, t_offset=i * visit_frames * DT)

    bins = estimator.get_speed_bins_state(CP)['bins']
    assert bins[1]['validBlocks'] >= 1 and bins[2]['validBlocks'] >= 1
    assert bins[1]['lateralDelayEstimate'] == pytest.approx(4 * DT, abs=0.01)
    assert bins[2]['lateralDelayEstimate'] == pytest.approx(6 * DT, abs=0.01)

  def test_speed_bins_persist(self):
    params = Params()
    CP = get_test_car_params()
    estimator = LateralLagEstimator(CP, DT)
    estimator.reset(0.3, BLOCK_NUM_NEEDED, [None, (0.2, BLOCK_NUM_NEEDED), (0.4, 2), None, None, (0.5, 1)])
    state = estimator.get_speed_bins_state(CP)
    params.put(SPEED_BINS_PARAM, state)

    bin_states = retrieve_speed_bins(params, CP)
    assert bin_states is not None
    assert [s[1] for s in bin_states] == [0, BLOCK_NUM_NEEDED, 2, 0, 0, 1]
    assert bin_states[1][0] == pytest.approx(0.2)

    # an invalid bin is dropped on its own; a different car, version or layout drops the table
    state['bins'][1]['status'] = 'invalid'
    params.put(SPEED_BINS_PARAM, state)
    assert retrieve_speed_bins(params, CP)[1] is None
    for change in ({'carFingerprint': 'other-car'}, {'version': VERSION + 1}, {'bins': state['bins'][:2]}):
      params.put(SPEED_BINS_PARAM, {**state, **change})
      assert retrieve_speed_bins(params, CP) is None
      assert params.get(SPEED_BINS_PARAM) is None

  @unittest.skipIf(PC, "only on device")
  def test_estimator_performance(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(mocked_CP, DT)

    ds = []
    for _ in range(1000):
      st = time.perf_counter()
      estimator.update_points()
      estimator.update_estimate()
      d = time.perf_counter() - st
      ds.append(d)

    assert np.mean(ds) < DT
