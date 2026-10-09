#!/usr/bin/env python3
import os
import numpy as np
import capnp
from collections import deque
from functools import partial

import openpilot.cereal.messaging as messaging
from openpilot.cereal import log
from opendbc.car.structs import car
from openpilot.cereal.services import SERVICE_LIST
from openpilot.common.params import Params
from openpilot.starpilot.schema_cache import get_cache, prewarm_cache_contracts, put_cache
from openpilot.common.realtime import config_realtime_process
from openpilot.common.swaglog import cloudlog
from openpilot.selfdrive.locationd.helpers import PoseCalibrator, Pose, fft_next_good_size, parabolic_peak_interp

BLOCK_SIZE = 100
BLOCK_NUM = 50
BLOCK_NUM_NEEDED = 5
MOVING_WINDOW_SEC = 60.0
MIN_OKAY_WINDOW_SEC = 25.0
MIN_RECOVERY_BUFFER_SEC = 2.0
MIN_VEGO = 8.0
MIN_ABS_YAW_RATE = 0.0
MAX_YAW_RATE_SANITY_CHECK = 1.0
MIN_NCC = 0.95
MAX_LAG = 0.65
MIN_LAG = 0.15
MAX_LAG_STD = 0.1
MAX_LAT_ACCEL = 2.0
MAX_LAT_ACCEL_DIFF = 0.6
MIN_LAT_ACCEL_RANGE = 0.5
MIN_CONFIDENCE = 0.7
CORR_BORDER_OFFSET = 5
LAG_CANDIDATE_CORR_THRESHOLD = 0.9
SMOOTH_K = 5
SMOOTH_SIGMA = 1.0

# Speed bins (m/s). Each bin learns its own delay, and lateralDelay is interpolated between the bin
# speeds at the current vEgo. The edges are 4.5 m/s apart and sit halfway between 5 mph steps
# (19.0 m/s = 42.5 mph), so steady cruising at a common set speed never straddles two bins.
SPEED_BIN_EDGES = [14.5, 19.0, 23.5, 28.0, 32.5]
SPEED_BIN_SPEEDS = [11.25, 16.75, 21.25, 25.75, 30.25, 34.75]
SPEED_BIN_HYSTERESIS = 0.5
# A bin's window joins separate visits end to end. Masking the first MAX_LAG (plus smoothing) of each
# visit keeps the cross-correlation from pairing samples across that seam.
SPEED_BIN_ENTRY_BUFFER_SEC = 1.0

VERSION = 1  # bump this to invalidate old parameter caches


def masked_symmetric_moving_average(x: np.ndarray, mask: np.ndarray, k: int, sigma: float) -> np.ndarray:
  assert k >= 1 and k % 2 == 1, "k must be positive and odd"
  pad = k // 2
  i = np.arange(k) - pad
  w = np.exp(-0.5 * (i / sigma) ** 2)
  w /= w.sum()
  xp = np.pad(x * mask, pad, mode="edge")
  mp = np.pad(mask, pad, mode="edge")
  num = np.convolve(xp, w, mode="valid")
  den = np.convolve(mp, w, mode="valid")
  return np.divide(num, den, out=np.full_like(num, np.nan, dtype=np.float64), where=den != 0)

def masked_normalized_cross_correlation(expected_sig: np.ndarray, actual_sig: np.ndarray, mask: np.ndarray, n: int):
  """
  References:
    D. Padfield. "Masked FFT registration". In Proc. Computer Vision and
    Pattern Recognition, pp. 2918-2925 (2010).
    :DOI:`10.1109/CVPR.2010.5540032`
  """

  eps = np.finfo(np.float64).eps
  expected_sig = np.asarray(expected_sig, dtype=np.float64)
  actual_sig = np.asarray(actual_sig, dtype=np.float64)

  expected_sig[~mask] = 0.0
  actual_sig[~mask] = 0.0

  rotated_expected_sig = expected_sig[::-1]
  rotated_mask = mask[::-1]

  fft = partial(np.fft.fft, n=n)

  actual_sig_fft = fft(actual_sig)
  rotated_expected_sig_fft = fft(rotated_expected_sig)
  actual_mask_fft = fft(mask.astype(np.float64))
  rotated_mask_fft = fft(rotated_mask.astype(np.float64))

  number_overlap_masked_samples = np.fft.ifft(rotated_mask_fft * actual_mask_fft).real
  number_overlap_masked_samples[:] = np.round(number_overlap_masked_samples)
  number_overlap_masked_samples[:] = np.fmax(number_overlap_masked_samples, eps)
  masked_correlated_actual_fft = np.fft.ifft(rotated_mask_fft * actual_sig_fft).real
  masked_correlated_expected_fft = np.fft.ifft(actual_mask_fft * rotated_expected_sig_fft).real

  numerator = np.fft.ifft(rotated_expected_sig_fft * actual_sig_fft).real
  numerator -= masked_correlated_actual_fft * masked_correlated_expected_fft / number_overlap_masked_samples

  actual_squared_fft = fft(actual_sig ** 2)
  actual_sig_denom = np.fft.ifft(rotated_mask_fft * actual_squared_fft).real
  actual_sig_denom -= masked_correlated_actual_fft ** 2 / number_overlap_masked_samples
  actual_sig_denom[:] = np.fmax(actual_sig_denom, 0.0)

  rotated_expected_squared_fft = fft(rotated_expected_sig ** 2)
  expected_sig_denom = np.fft.ifft(actual_mask_fft * rotated_expected_squared_fft).real
  expected_sig_denom -= masked_correlated_expected_fft ** 2 / number_overlap_masked_samples
  expected_sig_denom[:] = np.fmax(expected_sig_denom, 0.0)

  denom = np.sqrt(actual_sig_denom * expected_sig_denom)

  # zero-out samples with very small denominators
  tol = 1e3 * eps * np.max(np.abs(denom), keepdims=True)
  nonzero_indices = denom > tol

  ncc = np.zeros_like(denom, dtype=np.float64)
  ncc[nonzero_indices] = numerator[nonzero_indices] / denom[nonzero_indices]
  np.clip(ncc, -1, 1, out=ncc)

  return ncc


class Points:
  def __init__(self, num_points: int):
    self.times = deque[float]([0.0] * num_points, maxlen=num_points)
    self.okay = deque[bool]([False] * num_points, maxlen=num_points)
    self.desired = deque[float]([0.0] * num_points, maxlen=num_points)
    self.actual = deque[float]([0.0] * num_points, maxlen=num_points)

  @property
  def num_points(self):
    return len(self.desired)

  @property
  def num_okay(self):
    return np.count_nonzero(self.okay)

  def update(self, t: float, desired: float, actual: float, okay: bool):
    self.times.append(t)
    self.okay.append(okay)
    self.desired.append(desired)
    self.actual.append(actual)

  def get(self) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    return np.array(self.times), np.array(self.desired), np.array(self.actual), np.array(self.okay)


class BlockAverage:
  def __init__(self, num_blocks: int, block_size: int, valid_blocks: int, initial_value: float):
    self.num_blocks = num_blocks
    self.block_size = block_size
    self.block_idx = valid_blocks % num_blocks
    self.idx = 0

    self.values = np.tile(initial_value, (num_blocks, 1))
    self.valid_blocks = valid_blocks

  def update(self, value: float):
    self.values[self.block_idx] = (self.idx * self.values[self.block_idx] + value) / (self.idx + 1)
    self.idx = (self.idx + 1) % self.block_size
    if self.idx == 0:
      self.block_idx = (self.block_idx + 1) % self.num_blocks
      self.valid_blocks = min(self.valid_blocks + 1, self.num_blocks)

  def get(self) -> tuple[float, float, float, float]:
    valid_block_idx = [i for i in range(self.valid_blocks) if i != self.block_idx]
    valid_and_current_idx = valid_block_idx + ([self.block_idx] if self.idx > 0 else [])

    if len(valid_block_idx) > 0:
      valid_mean = float(np.mean(self.values[valid_block_idx], axis=0).item())
      valid_std = float(np.std(self.values[valid_block_idx], axis=0).item())
    else:
      valid_mean, valid_std = float('nan'), float('nan')

    if len(valid_and_current_idx) > 0:
      current_mean = float(np.mean(self.values[valid_and_current_idx], axis=0).item())
      current_std = float(np.std(self.values[valid_and_current_idx], axis=0).item())
    else:
      current_mean, current_std = float('nan'), float('nan')

    return valid_mean, valid_std, current_mean, current_std


class SpeedBin:
  def __init__(self, speed: float, window_len: int, num_blocks: int, block_size: int, valid_blocks: int, initial_value: float):
    self.speed = speed
    self.points = Points(window_len)
    self.block_avg = BlockAverage(num_blocks, block_size, valid_blocks, initial_value)
    self.last_estimate_t = 0.0
    self.last_checked_t = 0.0


def speed_bin_idx(v_ego: float, prev_idx: int | None, min_vego: float) -> int | None:
  if v_ego <= min_vego:
    return None

  idx = int(np.searchsorted(SPEED_BIN_EDGES, v_ego, side='right'))
  if prev_idx is not None and idx != prev_idx:
    lo = SPEED_BIN_EDGES[prev_idx - 1] if prev_idx > 0 else -np.inf
    hi = SPEED_BIN_EDGES[prev_idx] if prev_idx < len(SPEED_BIN_EDGES) else np.inf
    if lo - SPEED_BIN_HYSTERESIS <= v_ego < hi + SPEED_BIN_HYSTERESIS:
      idx = prev_idx
  return idx


class LateralLagEstimator:
  inputs = {"carControl", "carState", "controlsState", "extrinsicsCalibration", "deviceMotion"}

  def __init__(self, CP: car.CarParams, dt: float,
               block_count: int = BLOCK_NUM, min_valid_block_count: int = BLOCK_NUM_NEEDED, block_size: int = BLOCK_SIZE,
               window_sec: float = MOVING_WINDOW_SEC, okay_window_sec: float = MIN_OKAY_WINDOW_SEC, min_recovery_buffer_sec: float = MIN_RECOVERY_BUFFER_SEC,
               min_vego: float = MIN_VEGO, min_yr: float = MIN_ABS_YAW_RATE, min_ncc: float = MIN_NCC,
               max_lat_accel: float = MAX_LAT_ACCEL, max_lat_accel_diff: float = MAX_LAT_ACCEL_DIFF, min_confidence: float = MIN_CONFIDENCE):
    self.dt = dt
    self.window_sec = window_sec
    self.okay_window_sec = okay_window_sec
    self.min_recovery_buffer_sec = min_recovery_buffer_sec
    self.initial_lag = CP.steerActuatorDelay + 0.2
    self.block_size = block_size
    self.block_count = block_count
    self.min_valid_block_count = min_valid_block_count
    self.min_vego = min_vego
    self.min_yr = min_yr
    self.min_ncc = min_ncc
    self.min_confidence = min_confidence
    self.max_lat_accel = max_lat_accel
    self.max_lat_accel_diff = max_lat_accel_diff

    self.t = 0.0
    self.lat_active = False
    self.steering_pressed = False
    self.steering_saturated = False
    self.desired_curvature = 0.0
    self.v_ego = 0.0
    self.yaw_rate = 0.0
    self.yaw_rate_std = 0.0
    self.pose_valid = False

    self.last_lat_inactive_t = 0.0
    self.last_steering_pressed_t = 0.0
    self.last_steering_saturated_t = 0.0
    self.last_pose_invalid_t = 0.0
    self.last_estimate_t = 0.0

    self.speed_bin = None
    self.speed_bin_entry_t = 0.0

    self.calibrator = PoseCalibrator()

    self.reset(self.initial_lag, 0)

  def reset(self, initial_lag: float, valid_blocks: int, bin_states: list[tuple[float, int] | None] | None = None):
    window_len = int(self.window_sec / self.dt)
    self.points = Points(window_len)
    self.block_avg = BlockAverage(self.block_count, self.block_size, valid_blocks, initial_lag)

    if bin_states is None:
      bin_states = [None] * len(SPEED_BIN_SPEEDS)
    self.speed_bins = []
    for speed, state in zip(SPEED_BIN_SPEEDS, bin_states, strict=True):
      bin_lag, bin_valid_blocks = state if state is not None else (initial_lag, 0)
      self.speed_bins.append(SpeedBin(speed, window_len, self.block_count, self.block_size, bin_valid_blocks, bin_lag))

  def get_status(self, block_avg: BlockAverage) -> log.LateralDelay.Status:
    valid_mean_lag, valid_std, _, _ = block_avg.get()
    if block_avg.valid_blocks >= self.min_valid_block_count and not np.isnan(valid_mean_lag) and not np.isnan(valid_std):
      if valid_std > MAX_LAG_STD:
        return log.LateralDelay.Status.invalid
      return log.LateralDelay.Status.estimated
    return log.LateralDelay.Status.unestimated

  def get_bin_lag(self, speed_bin: SpeedBin, fallback_lag: float) -> float:
    # shrink a bin toward the all-speed value until it has learned enough blocks of its own
    valid_mean_lag, valid_std, _, _ = speed_bin.block_avg.get()
    if np.isnan(valid_mean_lag) or valid_std > MAX_LAG_STD:
      return fallback_lag
    weight = min(speed_bin.block_avg.valid_blocks / self.min_valid_block_count, 1.0)
    return weight * min(MAX_LAG, max(MIN_LAG, valid_mean_lag)) + (1.0 - weight) * fallback_lag

  def get_msg(self, valid: bool, debug: bool = False) -> capnp._DynamicStructBuilder:
    msg = messaging.new_message('lateralDelay')

    msg.valid = valid

    lateralDelay = msg.lateralDelay

    valid_mean_lag, _, current_mean_lag, current_std = self.block_avg.get()
    lateralDelay.status = self.get_status(self.block_avg)

    if lateralDelay.status == log.LateralDelay.Status.estimated:
      global_lag = min(MAX_LAG, max(MIN_LAG, valid_mean_lag))
    else:
      global_lag = self.initial_lag
    bin_lags = [self.get_bin_lag(speed_bin, global_lag) for speed_bin in self.speed_bins]
    lateralDelay.lateralDelay = float(np.interp(self.v_ego, SPEED_BIN_SPEEDS, bin_lags))

    lateralDelay.lateralDelayEstimate, lateralDelay.lateralDelayEstimateStd = self.get_estimate(current_mean_lag, current_std)

    lateralDelay.validBlocks = self.block_avg.valid_blocks
    lateralDelay.calPerc = min(100 * (self.block_avg.valid_blocks * self.block_size + self.block_avg.idx) //
                            (self.min_valid_block_count * self.block_size), 100)

    speed_bins = lateralDelay.init('speedBins', len(self.speed_bins))
    for msg_bin, speed_bin, bin_lag in zip(speed_bins, self.speed_bins, bin_lags, strict=True):
      _, _, bin_current_mean_lag, bin_current_std = speed_bin.block_avg.get()
      msg_bin.speed = speed_bin.speed
      msg_bin.lateralDelay = bin_lag
      msg_bin.lateralDelayEstimate, msg_bin.lateralDelayEstimateStd = self.get_estimate(bin_current_mean_lag, bin_current_std)
      msg_bin.validBlocks = speed_bin.block_avg.valid_blocks
      msg_bin.status = self.get_status(speed_bin.block_avg)

    if debug:
      lateralDelay.points = self.block_avg.values.flatten().tolist()
    lateralDelay.version = VERSION

    return msg

  def get_estimate(self, current_mean_lag: float, current_std: float) -> tuple[float, float]:
    if not np.isnan(current_mean_lag) and not np.isnan(current_std):
      return current_mean_lag, current_std
    return self.initial_lag, 0.0

  def handle_log(self, t: float, which: str, msg: capnp._DynamicStructReader):
    if which == "carControl":
      self.lat_active = msg.latActive
    elif which == "carState":
      self.steering_pressed = msg.steeringPressed
      self.v_ego = msg.vEgo
    elif which == "controlsState":
      self.steering_saturated = getattr(msg.lateralControlState, msg.lateralControlState.which()).saturated
      self.desired_curvature = msg.desiredCurvature
    elif which == "extrinsicsCalibration":
      self.calibrator.feed_extrinsics_calibration(msg)
    elif which == "deviceMotion":
      device_motion = Pose.from_device_motion(msg)
      calibrated_pose = self.calibrator.build_calibrated_pose(device_motion)
      self.yaw_rate = calibrated_pose.angular_velocity.yaw
      self.yaw_rate_std = calibrated_pose.angular_velocity.yaw_std
      self.pose_valid = msg.angularVelocityDevice.valid and msg.posenetOK and msg.inputsOK
    self.t = t

  def points_enough(self, points: Points):
    return points.num_points >= int(self.okay_window_sec / self.dt)

  def points_valid(self, points: Points):
    return points.num_okay >= int(self.okay_window_sec / self.dt)

  def update_points(self):
    la_desired = self.desired_curvature * self.v_ego * self.v_ego
    la_actual_pose = self.yaw_rate * self.v_ego

    fast = self.v_ego > self.min_vego
    turning = np.abs(self.yaw_rate) >= self.min_yr
    sensors_valid = self.pose_valid and np.abs(self.yaw_rate) < MAX_YAW_RATE_SANITY_CHECK and self.yaw_rate_std < MAX_YAW_RATE_SANITY_CHECK
    la_valid = np.abs(la_actual_pose) <= self.max_lat_accel and np.abs(la_desired - la_actual_pose) <= self.max_lat_accel_diff
    calib_valid = self.calibrator.calib_valid

    if not self.lat_active:
      self.last_lat_inactive_t = self.t
    if self.steering_pressed:
      self.last_steering_pressed_t = self.t
    if self.steering_saturated:
      self.last_steering_saturated_t = self.t
    if not sensors_valid or not la_valid:
      self.last_pose_invalid_t = self.t

    has_recovered = all( # wait for recovery after !lat_active, steering_pressed, steering_saturated, !sensors/la_valid
      self.t - last_t >= self.min_recovery_buffer_sec
      for last_t in [self.last_lat_inactive_t, self.last_steering_pressed_t, self.last_steering_saturated_t, self.last_pose_invalid_t]
    )
    okay = self.lat_active and not self.steering_pressed and not self.steering_saturated and \
           fast and turning and has_recovered and calib_valid and sensors_valid and la_valid

    self.points.update(self.t, la_desired, la_actual_pose, okay)

    # each bin only sees the samples taken while driving in its speed range
    speed_bin = speed_bin_idx(self.v_ego, self.speed_bin, self.min_vego)
    if speed_bin != self.speed_bin:
      self.speed_bin = speed_bin
      self.speed_bin_entry_t = self.t
    if speed_bin is not None:
      bin_okay = okay and self.t - self.speed_bin_entry_t >= SPEED_BIN_ENTRY_BUFFER_SEC
      self.speed_bins[speed_bin].points.update(self.t, la_desired, la_actual_pose, bin_okay)

  def update_estimate(self):
    if (delay := self.estimate(self.points, self.last_estimate_t)) is not None:
      self.block_avg.update(delay)
      self.last_estimate_t = self.t

    for speed_bin in self.speed_bins:
      # bins we aren't driving in don't change, so only re-check one once it has new samples
      if speed_bin.points.times[-1] <= speed_bin.last_checked_t:
        continue
      speed_bin.last_checked_t = speed_bin.points.times[-1]
      if (delay := self.estimate(speed_bin.points, speed_bin.last_estimate_t)) is not None:
        speed_bin.block_avg.update(delay)
        speed_bin.last_estimate_t = self.t

  def estimate(self, points: Points, last_estimate_t: float) -> float | None:
    if not self.points_enough(points):
      return None

    times, desired, actual, okay = points.get()
    # check if there are any new valid data points since the last update
    is_valid = self.points_valid(points) and (actual.max() - actual.min() >= MIN_LAT_ACCEL_RANGE)
    if last_estimate_t != 0 and times[0] <= last_estimate_t:
      new_values_start_idx = next(-i for i, t in enumerate(reversed(times)) if t <= last_estimate_t)
      is_valid = is_valid and not (new_values_start_idx == 0 or not np.any(okay[new_values_start_idx:]))
    if not is_valid:
      return None

    desired = masked_symmetric_moving_average(desired, okay, SMOOTH_K, SMOOTH_SIGMA)
    actual = masked_symmetric_moving_average(actual, okay, SMOOTH_K, SMOOTH_SIGMA)

    delay, corr, confidence = self.actuator_delay(desired, actual, okay, self.dt, MIN_LAG, MAX_LAG)
    if corr < self.min_ncc or confidence < self.min_confidence:
      return None
    return delay

  @staticmethod
  def actuator_delay(expected_sig: np.ndarray, actual_sig: np.ndarray, mask: np.ndarray,
                     dt: float, min_lag: float, max_lag: float) -> tuple[float, float, float]:
    assert len(expected_sig) == len(actual_sig)
    min_lag_samples, max_lag_samples, one_sec_samples = int(round(min_lag / dt)), int(round(max_lag / dt)), int(round(1.0 / dt))
    padded_size = fft_next_good_size(len(expected_sig) + max(max_lag_samples, one_sec_samples))

    ncc = masked_normalized_cross_correlation(expected_sig, actual_sig, mask, padded_size)

    # only consider lags from ranges:
    roi = np.s_[len(expected_sig) - 1 + min_lag_samples: len(expected_sig) - 1 + max_lag_samples] # min_lag - max_lag range
    threshold_roi = np.s_[len(expected_sig) - 1: len(expected_sig) - 1 + one_sec_samples] # 0 - 1 second range
    confidence_roi = np.s_[threshold_roi.start - CORR_BORDER_OFFSET: threshold_roi.stop + CORR_BORDER_OFFSET] # threshold range +/- border
    roi_ncc, confidence_roi_ncc, threshold_roi_ncc = ncc[roi], ncc[confidence_roi], ncc[threshold_roi]

    max_corr_index = np.argmax(roi_ncc)
    corr = roi_ncc[max_corr_index]
    lag = parabolic_peak_interp(roi_ncc, max_corr_index) * dt + min_lag

    # to estimate lag confidence, gather all high-correlation candidates and see how spread they are
    # if e.g. 0.8 and 0.4 are both viable, this is an ambiguous case
    ncc_thresh = (threshold_roi_ncc.max() - threshold_roi_ncc.min()) * LAG_CANDIDATE_CORR_THRESHOLD + threshold_roi_ncc.min()
    good_lag_candidate_mask = confidence_roi_ncc >= ncc_thresh
    good_lag_candidate_edges = np.diff(good_lag_candidate_mask.astype(int), prepend=0, append=0)
    starts, ends = np.where(good_lag_candidate_edges == 1)[0], np.where(good_lag_candidate_edges == -1)[0] - 1
    run_idx = np.searchsorted(starts, max_corr_index + CORR_BORDER_OFFSET, side='right') - 1
    width = ends[run_idx] - starts[run_idx] + 1
    confidence = np.clip(1 - width * dt, 0, 1)

    return lag, corr, confidence


def retrieve_initial_lag(params: Params, CP: car.CarParams):
  last_lag_data = get_cache(params, "LiveDelay")
  last_carparams_data = get_cache(params, "CarParamsPrevRoute")

  if last_lag_data is not None and last_carparams_data is not None:
    try:
      with log.Event.from_bytes(last_lag_data) as last_lag_msg, car.CarParams.from_bytes(last_carparams_data) as last_CP:
        ld = last_lag_msg.lateralDelay
        if last_CP.carFingerprint != CP.carFingerprint:
          raise Exception("Car model mismatch")

        lag, valid_blocks, status, version = ld.lateralDelayEstimate, ld.validBlocks, ld.status, ld.version
        assert valid_blocks <= BLOCK_NUM, "Invalid number of valid blocks"
        assert status != log.LateralDelay.Status.invalid, "Lag estimate is invalid"
        assert version == VERSION, f"Lag estimate is from a different version (got {version}, expected {VERSION})"

        # saved bins are only reused if they were learned with the same bin layout
        bin_states = None
        saved_speeds = [b.speed for b in ld.speedBins]
        if len(saved_speeds) == len(SPEED_BIN_SPEEDS) and np.allclose(saved_speeds, SPEED_BIN_SPEEDS):
          bin_states = [(b.lateralDelayEstimate, b.validBlocks)
                        if 0 <= b.validBlocks <= BLOCK_NUM and b.status != log.LateralDelay.Status.invalid else None
                        for b in ld.speedBins]
        return lag, valid_blocks, bin_states
    except Exception as e:
      cloudlog.error(f"Failed to retrieve initial lag: {e}")
      params.remove("LiveDelay")

  return None


def main():
  prewarm_cache_contracts()
  config_realtime_process([0, 1, 2, 3], 5)

  DEBUG = bool(int(os.getenv("DEBUG", "0")))

  pm = messaging.PubMaster(['lateralDelay'])
  sm = messaging.SubMaster(['deviceMotion', 'extrinsicsCalibration', 'carState', 'controlsState', 'carControl'], poll='deviceMotion')

  params = Params()
  CP = messaging.log_from_bytes(params.get("CarParams", block=True), car.CarParams)

  lag_learner = LateralLagEstimator(CP, 1. / SERVICE_LIST['deviceMotion'].frequency)
  from openpilot.starpilot.lateral.gm_geometry_runtime import GeometryPublicationOwner
  geometry_owner = GeometryPublicationOwner(params, CP)
  if (initial_lag_params := retrieve_initial_lag(params, CP)) is not None:
    lag, valid_blocks, bin_states = initial_lag_params
    lag_learner.reset(lag, valid_blocks, bin_states)

  while True:
    sm.update()
    if sm.all_checks():
      for which in sorted(sm.updated.keys(), key=lambda x: sm.logMonoTime[x]):
        if sm.updated[which]:
          t = sm.logMonoTime[which] * 1e-9
          lag_learner.handle_log(t, which, sm[which])
      lag_learner.update_points()

    # 4Hz driven by deviceMotion
    if sm.frame % 5 == 0:
      lag_learner.update_estimate()
      lag_msg = lag_learner.get_msg(sm.all_checks(), DEBUG)
      lag_msg_dat = geometry_owner.delay(lag_msg).to_bytes()
      pm.send('lateralDelay', lag_msg_dat)

      if sm.frame % 1200 == 0: # cache every 60 seconds
        put_cache(params, "LiveDelay", lag_msg)
