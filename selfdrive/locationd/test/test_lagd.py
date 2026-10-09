import random
import time
from types import SimpleNamespace

import numpy as np
import pytest

from cereal import messaging, log, car
from openpilot.selfdrive.locationd.lagd import LateralLagEstimator, retrieve_initial_lag, masked_normalized_cross_correlation, speed_bin_idx, \
                                               BLOCK_NUM_NEEDED, BLOCK_SIZE, MIN_OKAY_WINDOW_SEC, MAX_LAG, MIN_VEGO, SPEED_BIN_SPEEDS, \
                                               SPEED_BIN_ENTRY_BUFFER_SEC
from openpilot.selfdrive.test.process_replay.migration import migrate, migrate_carParams
from openpilot.selfdrive.locationd.test.test_locationd_scenarios import TEST_ROUTE
from openpilot.common.params import Params
from openpilot.tools.lib.logreader import LogReader
from openpilot.system.hardware import PC

MAX_ERR_FRAMES = 1
DT = 0.05
LAGD_MAX_LAG_FRAMES = int(round(MAX_LAG / DT))


def process_messages(estimator, lag_frames, n_frames, vego=20.0, rejection_threshold=0.0, t_offset=0.0):
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
      (t, "livePose", log.LivePose(angularVelocityDevice=log.LivePose.XYZMeasurement(z=actual_yr, valid=True),
                                   posenetOK=True, inputsOK=True)),
      (t, "liveCalibration", log.LiveCalibrationData(rpyCalib=[0, 0, 0], calStatus=log.LiveCalibrationData.Status.calibrated)),
    ]
    for t, w, m in msgs:
      estimator.handle_log(t, w, m)
    estimator.update_points()
    estimator.update_estimate()


class TestLagd:
  def test_manual_delay_uses_exact_configured_value(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.11)
    estimator = LateralLagEstimator(mocked_CP, DT)
    estimator.starpilot_toggles = SimpleNamespace(
      use_custom_steerActuatorDelay=True,
      steerActuatorDelay=0.30,
    )

    msg = estimator.get_msg(True)

    assert msg.liveDelay.lateralDelay == pytest.approx(0.30)

  def test_read_saved_params(self):
    params = Params()

    lr = migrate(LogReader(TEST_ROUTE), [migrate_carParams])
    CP = next(m for m in lr if m.which() == "carParams").carParams

    msg = messaging.new_message('liveDelay')
    msg.liveDelay.lateralDelayEstimate = random.random()
    msg.liveDelay.validBlocks = random.randint(1, 10)
    speed_bins = msg.liveDelay.init('speedBins', len(SPEED_BIN_SPEEDS))
    for speed_bin, speed in zip(speed_bins, SPEED_BIN_SPEEDS, strict=True):
      speed_bin.speed = speed
      speed_bin.lateralDelayEstimate = random.random()
      speed_bin.validBlocks = random.randint(0, 10)
    speed_bins[0].status = 'invalid'
    params.put("LiveDelay", msg.to_bytes())
    params.put("CarParamsPrevRoute", CP.as_builder().to_bytes())

    saved_lag_params = retrieve_initial_lag(params, CP)
    assert saved_lag_params is not None

    lag, valid_blocks, bin_states = saved_lag_params
    assert lag == msg.liveDelay.lateralDelayEstimate
    assert valid_blocks == msg.liveDelay.validBlocks
    assert bin_states[0] is None
    for bin_state, speed_bin in zip(bin_states[1:], list(speed_bins)[1:], strict=True):
      assert bin_state == (speed_bin.lateralDelayEstimate, speed_bin.validBlocks)

    # bins learned with a different layout are dropped, the all-speed estimate is kept
    msg.liveDelay.init('speedBins', 2)
    params.put("LiveDelay", msg.to_bytes())
    lag, valid_blocks, bin_states = retrieve_initial_lag(params, CP)
    assert lag == msg.liveDelay.lateralDelayEstimate
    assert bin_states is None

  def test_ncc(self):
    lag_frames = random.randint(1, 19)

    desired_sig = np.sin(np.arange(0.0, 10.0, 0.1))
    actual_sig = np.sin(np.arange(0.0, 10.0, 0.1) - lag_frames * 0.1)
    mask = np.ones(len(desired_sig), dtype=bool)

    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr) == lag_frames

    # add some noise
    desired_sig += np.random.normal(0, 0.05, len(desired_sig))
    actual_sig += np.random.normal(0, 0.05, len(actual_sig))
    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr)  in range(lag_frames - MAX_ERR_FRAMES, lag_frames + MAX_ERR_FRAMES + 1)

    # mask out 40% of the values, and make them noise
    mask = np.random.choice([True, False], size=len(desired_sig), p=[0.6, 0.4])
    desired_sig[~mask] = np.random.normal(0, 1, size=np.sum(~mask))
    actual_sig[~mask] = np.random.normal(0, 1, size=np.sum(~mask))
    corr = masked_normalized_cross_correlation(desired_sig, actual_sig, mask, 200)[len(desired_sig) - 1:len(desired_sig) + 20]
    assert np.argmax(corr) in range(lag_frames - MAX_ERR_FRAMES, lag_frames + MAX_ERR_FRAMES + 1)

  def test_empty_estimator(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(mocked_CP, DT)
    estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)
    msg = estimator.get_msg(True)
    assert msg.liveDelay.status == 'unestimated'
    assert np.allclose(msg.liveDelay.lateralDelay, estimator.initial_lag)
    assert np.allclose(msg.liveDelay.lateralDelayEstimate, estimator.initial_lag)
    assert msg.liveDelay.validBlocks == 0
    assert msg.liveDelay.calPerc == 0

  def test_estimator_basics(self, subtests):
    for lag_frames in range(LAGD_MAX_LAG_FRAMES - 1):
      with subtests.test(msg=f"lag_frames={lag_frames}"):
        mocked_CP = car.CarParams(steerActuatorDelay=0.5)
        estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)
        estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)
        process_messages(estimator, lag_frames, int(MIN_OKAY_WINDOW_SEC / DT) + BLOCK_NUM_NEEDED * BLOCK_SIZE)
        msg = estimator.get_msg(True)
        assert msg.liveDelay.status == 'estimated'
        assert np.allclose(msg.liveDelay.lateralDelay, lag_frames * DT, atol=0.01)
        assert np.allclose(msg.liveDelay.lateralDelayEstimate, lag_frames * DT, atol=0.01)
        assert np.allclose(msg.liveDelay.lateralDelayEstimateStd, 0.0, atol=0.01)
        assert msg.liveDelay.validBlocks == BLOCK_NUM_NEEDED
        assert msg.liveDelay.calPerc == 100

  def test_estimator_masking(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    lag_frames = random.randint(1, LAGD_MAX_LAG_FRAMES - 1)
    estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0, min_valid_block_count=1)
    estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)
    process_messages(estimator, lag_frames, (int(MIN_OKAY_WINDOW_SEC / DT) + BLOCK_SIZE) * 2, rejection_threshold=0.4)
    msg = estimator.get_msg(True)
    assert np.allclose(msg.liveDelay.lateralDelayEstimate, lag_frames * DT, atol=0.01)
    assert np.allclose(msg.liveDelay.lateralDelayEstimateStd, 0.0, atol=0.01)
    assert msg.liveDelay.calPerc == 100

  def test_speed_bin_hysteresis(self):
    assert speed_bin_idx(MIN_VEGO - 0.1, None, MIN_VEGO) is None
    assert speed_bin_idx(MIN_VEGO + 0.1, None, MIN_VEGO) == 0
    assert speed_bin_idx(40.0, None, MIN_VEGO) == len(SPEED_BIN_SPEEDS) - 1

    # jitter around the 19.0 m/s edge stays in whichever bin it started in
    for start_v, expected_bin in ((19.2, 2), (18.8, 1)):
      speed_bin = speed_bin_idx(start_v, None, MIN_VEGO)
      for v in (19.3, 18.7, 19.2, 18.8, 19.4, 18.6):
        speed_bin = speed_bin_idx(v, speed_bin, MIN_VEGO)
        assert speed_bin == expected_bin

    assert speed_bin_idx(18.4, 2, MIN_VEGO) == 1
    assert speed_bin_idx(19.6, 1, MIN_VEGO) == 2

  def test_speed_bin_interpolation(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.1)
    estimator = LateralLagEstimator(mocked_CP, DT)
    estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)
    estimator.reset(0.3, BLOCK_NUM_NEEDED, [None, (0.2, BLOCK_NUM_NEEDED), (0.4, 2), None, None, None])

    def lateral_delay_at(v_ego):
      estimator.handle_log(0.0, "carState", car.CarState(vEgo=v_ego))
      return estimator.get_msg(True).liveDelay.lateralDelay

    partial_bin_lag = 2 / BLOCK_NUM_NEEDED * 0.4 + (1 - 2 / BLOCK_NUM_NEEDED) * 0.3
    assert lateral_delay_at(SPEED_BIN_SPEEDS[0]) == pytest.approx(0.3)
    assert lateral_delay_at(SPEED_BIN_SPEEDS[1]) == pytest.approx(0.2)
    assert lateral_delay_at(SPEED_BIN_SPEEDS[2]) == pytest.approx(partial_bin_lag)
    assert lateral_delay_at((SPEED_BIN_SPEEDS[0] + SPEED_BIN_SPEEDS[1]) / 2) == pytest.approx(0.25)
    assert lateral_delay_at((SPEED_BIN_SPEEDS[1] + SPEED_BIN_SPEEDS[2]) / 2) == pytest.approx((0.2 + partial_bin_lag) / 2)
    assert lateral_delay_at(0.0) == pytest.approx(0.3)
    assert lateral_delay_at(50.0) == pytest.approx(0.3)

    msg = estimator.get_msg(True)
    assert [b.speed for b in msg.liveDelay.speedBins] == pytest.approx(SPEED_BIN_SPEEDS)
    assert [b.lateralDelay for b in msg.liveDelay.speedBins] == pytest.approx([0.3, 0.2, partial_bin_lag, 0.3, 0.3, 0.3])
    assert [b.validBlocks for b in msg.liveDelay.speedBins] == [0, BLOCK_NUM_NEEDED, 2, 0, 0, 0]
    assert msg.liveDelay.speedBins[1].status == 'estimated'
    assert msg.liveDelay.speedBins[2].status == 'unestimated'

  def test_speed_bins_learn_separately(self):
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)
    estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)

    n_frames = int((MIN_OKAY_WINDOW_SEC + 2 * SPEED_BIN_ENTRY_BUFFER_SEC) / DT) + BLOCK_NUM_NEEDED * BLOCK_SIZE
    process_messages(estimator, 3, n_frames, vego=SPEED_BIN_SPEEDS[1])
    process_messages(estimator, 5, n_frames, vego=SPEED_BIN_SPEEDS[2], t_offset=n_frames * DT)

    msg = estimator.get_msg(True)
    bins = msg.liveDelay.speedBins
    assert bins[1].status == 'estimated' and bins[2].status == 'estimated'
    assert bins[1].lateralDelayEstimate == pytest.approx(3 * DT, abs=0.01)
    assert bins[2].lateralDelayEstimate == pytest.approx(5 * DT, abs=0.01)

    # bins with no data of their own follow the all-speed value
    global_lag = estimator.block_avg.get()[0] if msg.liveDelay.status == 'estimated' else estimator.initial_lag
    for i in (0, 3, 4, 5):
      assert bins[i].validBlocks == 0
      assert bins[i].lateralDelay == pytest.approx(global_lag)

    estimator.handle_log(0.0, "carState", car.CarState(vEgo=(SPEED_BIN_SPEEDS[1] + SPEED_BIN_SPEEDS[2]) / 2))
    assert estimator.get_msg(True).liveDelay.lateralDelay == pytest.approx(4 * DT, abs=0.01)

  def test_speed_bins_alternating(self):
    # short alternating visits: each bin's window joins its visits end to end, and the entry buffer
    # keeps the correlation from pairing samples across those seams
    mocked_CP = car.CarParams(steerActuatorDelay=0.5)
    estimator = LateralLagEstimator(mocked_CP, DT, min_recovery_buffer_sec=0.0, min_yr=0.0)
    estimator.starpilot_toggles = SimpleNamespace(use_custom_steerActuatorDelay=False)

    visit_frames = int(2.0 / DT)
    for i in range(60):
      lag_frames, vego = ((3, SPEED_BIN_SPEEDS[1]), (5, SPEED_BIN_SPEEDS[2]))[i % 2]
      process_messages(estimator, lag_frames, visit_frames, vego=vego, t_offset=i * visit_frames * DT)

    bins = estimator.get_msg(True).liveDelay.speedBins
    assert bins[1].validBlocks >= 1 and bins[2].validBlocks >= 1
    assert bins[1].lateralDelayEstimate == pytest.approx(3 * DT, abs=0.01)
    assert bins[2].lateralDelayEstimate == pytest.approx(5 * DT, abs=0.01)

  @pytest.mark.skipif(PC, reason="only on device")
  @pytest.mark.timeout(60)
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
