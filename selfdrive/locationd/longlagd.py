#!/usr/bin/env python3
"""Learns the longitudinal actuator delay separately for gas and brake.

The powertrain meets a request down to the decel the car gets from just closing the throttle
(the brake threshold); anything past that needs the brakes, which have to build pressure first.
So the response is modelled as two paths, each with its own delay and gain:

  accel(t) = g_gas * max(m(t - d_gas), 0) + g_brake * min(m(t - d_brake), 0) + slow terms
  m(t)     = accel command - brake threshold at the current speed

and (d_gas, d_brake) are found by an exhaustive search over the lag grid, with the gains solved
in closed form at each point. Correlating each regime separately (as lagd does for lateral)
was tried first, and pulled the two delays towards each other: the samples around every
gas/brake transition belong to both.

The response is a zero-phase derivative of vEgoRaw. Each estimate is a batch over the buffered
window, so a non-causal filter costs nothing, and it keeps the filter lag in aEgo (a Kalman
filter) and livePose (an EKF) out of the measured delay.
"""
import os
from collections import deque
from dataclasses import dataclass

import capnp
import numpy as np

import cereal.messaging as messaging
from cereal import car, log
from cereal.services import SERVICE_LIST
from openpilot.common.params import Params
from openpilot.common.realtime import config_realtime_process
from openpilot.common.swaglog import cloudlog
from openpilot.selfdrive.locationd.helpers import parabolic_peak_interp
from openpilot.selfdrive.locationd.lagd import BlockAverage, masked_symmetric_moving_average
from openpilot.starpilot.common.longitudinal_delay import MIN_LONG_DELAY, MAX_LONG_DELAY, LongDelayStatus, brake_threshold

LongCtrlState = car.CarControl.Actuators.LongControlState

BLOCK_NUM = 50
BLOCK_NUM_NEEDED = 5
WINDOW_SEC = 120.0
MIN_RECOVERY_BUFFER_SEC = 2.0
MIN_VEGO = 5.0
MAX_ABS_ACCEL = 4.0
MAX_LAG = MAX_LONG_DELAY
MAX_LAG_STD = 0.1
# Share of the (band-passed) accel the two-path model has to explain.
MIN_R2 = 0.7
# How much worse the fit has to get, as a share of the accel's energy, when a delay is moved
# SHARPNESS_SEC either way. A flat cost means the window can't pin that delay down.
MIN_SHARPNESS = 0.003
SHARPNESS_SEC = 0.1
MIN_GAIN, MAX_GAIN = 0.5, 2.0
# Requests this close to the brake threshold don't count as data for either regime.
REGIME_DEADBAND = 0.1
# Zero-phase low-pass, ~0.1s std: vehicle accel has nothing above a couple of Hz.
SMOOTH_K = 9
SMOOTH_SIGMA = 2.0
# Zero-phase high-pass, ~1.5s std: removes grade. The integrator commands extra accel to hold
# speed on a hill that the wheels never show, and that offset is no part of the delay.
DETREND_K = 121
DETREND_SIGMA = 30.0


@dataclass(frozen=True)
class Regime:
  name: str
  sign: int  # +1 above the brake threshold, -1 below it
  block_size: int
  min_okay_sec: float
  min_cmd_range: float


# Braking is a small share of driving, so the brake regime is allowed to estimate from less data.
REGIMES = (
  Regime("gas", 1, block_size=100, min_okay_sec=20.0, min_cmd_range=0.3),
  Regime("brake", -1, block_size=40, min_okay_sec=8.0, min_cmd_range=0.3),
)


@dataclass
class RegimeFit:
  delay: float
  gain: float
  sharpness: float
  seconds: float
  cmd_range: float


@dataclass
class SplitFit:
  r2: float
  regimes: dict[str, RegimeFit]


class Points:
  def __init__(self, num_points: int):
    self.times = deque[float](maxlen=num_points)
    self.okay = deque[bool](maxlen=num_points)
    self.cmd = deque[float](maxlen=num_points)
    self.v = deque[float](maxlen=num_points)
    self.thr = deque[float](maxlen=num_points)

  @property
  def num_points(self):
    return len(self.times)

  def update(self, t: float, cmd: float, v: float, thr: float, okay: bool):
    self.times.append(t)
    self.okay.append(okay)
    self.cmd.append(cmd)
    self.v.append(v)
    self.thr.append(thr)

  def get(self) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    return np.array(self.times), np.array(self.cmd), np.array(self.v), np.array(self.thr), np.array(self.okay, dtype=bool)


def true_accel(times: np.ndarray, v: np.ndarray, okay: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
  """Central-difference accel from wheel speed. A sample is only usable when both neighbours
  were too, since the difference spans them."""
  accel = np.zeros_like(v)
  usable = okay.copy()
  if len(v) < 3:
    return accel, np.zeros_like(okay)
  accel[1:-1] = (v[2:] - v[:-2]) / np.maximum(times[2:] - times[:-2], 1e-3)
  usable[0] = usable[-1] = False
  usable[1:-1] &= okay[:-2] & okay[2:]
  return accel, usable


def band_pass(x: np.ndarray, mask: np.ndarray) -> np.ndarray:
  x = np.nan_to_num(masked_symmetric_moving_average(x, mask, SMOOTH_K, SMOOTH_SIGMA))
  return x - np.nan_to_num(masked_symmetric_moving_average(x, mask, DETREND_K, DETREND_SIGMA))


def _profile_peak(cost: np.ndarray, k: int, step: int, energy: float) -> tuple[float, float]:
  """Sub-sample minimum of a 1D cost profile, and how sharply the cost rises around it."""
  if k < step or k > len(cost) - 1 - step:
    return float(k), 0.0
  sharpness = (min(cost[k - step], cost[k + step]) - cost[k]) / energy
  return float(parabolic_peak_interp(-cost, k)), float(sharpness)


def fit_split_delay(cmd: np.ndarray, accel: np.ndarray, thr: np.ndarray, usable: np.ndarray,
                    dt: float, max_lag: float = MAX_LAG) -> SplitFit | None:
  n_lag = int(round(max_lag / dt))
  margin = cmd - thr
  y = band_pass(accel - thr, usable)
  paths = {"gas": band_pass(np.maximum(margin, 0.0), usable), "brake": band_pass(np.minimum(margin, 0.0), usable)}

  # score only samples whose whole lag history was usable
  history = np.convolve(usable.astype(int), np.ones(n_lag + 1, dtype=int))[:len(usable)]
  idx = np.flatnonzero(history == n_lag + 1)
  if len(idx) < 2 * (n_lag + 1):
    return None

  lags = np.arange(n_lag + 1)
  G = paths["gas"][idx[None, :] - lags[:, None]]
  B = paths["brake"][idx[None, :] - lags[:, None]]
  y = y[idx]

  energy = float(y @ y)
  if energy <= 0.0:
    return None
  # least squares for both gains at every (gas lag, brake lag) pair, from the Gram terms
  reg = 1e-6 * energy
  gg = np.einsum('ij,ij->i', G, G)[:, None] + reg
  bb = np.einsum('ij,ij->i', B, B)[None, :] + reg
  gb = G @ B.T
  yg = (G @ y)[:, None]
  yb = (B @ y)[None, :]
  det = gg * bb - gb * gb
  cost = energy - (bb * yg * yg - 2 * gb * yg * yb + gg * yb * yb) / det

  kg, kb = np.unravel_index(np.argmin(cost), cost.shape)
  det_min = det[kg, kb]
  gains = {
    "gas": float((bb[0, kb] * yg[kg, 0] - gb[kg, kb] * yb[0, kb]) / det_min),
    "brake": float((gg[kg, 0] * yb[0, kb] - gb[kg, kb] * yg[kg, 0]) / det_min),
  }
  step = max(1, int(round(SHARPNESS_SEC / dt)))
  peaks = {"gas": _profile_peak(cost[:, kb], kg, step, energy), "brake": _profile_peak(cost[kg, :], kb, step, energy)}

  margin = margin[idx]
  regimes = {}
  for regime in REGIMES:
    in_regime = margin * regime.sign > REGIME_DEADBAND
    k, sharpness = peaks[regime.name]
    regimes[regime.name] = RegimeFit(
      delay=k * dt,
      gain=gains[regime.name],
      sharpness=sharpness,
      seconds=float(np.count_nonzero(in_regime) * dt),
      cmd_range=float(np.ptp(margin[in_regime])) if np.any(in_regime) else 0.0,
    )
  return SplitFit(r2=float(1.0 - cost[kg, kb] / energy), regimes=regimes)


class LongitudinalLagEstimator:
  inputs = {"carControl", "carState", "starpilotCarControl"}

  def __init__(self, CP: car.CarParams, dt: float, window_sec: float = WINDOW_SEC,
               min_recovery_buffer_sec: float = MIN_RECOVERY_BUFFER_SEC, min_vego: float = MIN_VEGO,
               min_r2: float = MIN_R2, min_sharpness: float = MIN_SHARPNESS,
               min_valid_block_count: int = BLOCK_NUM_NEEDED):
    self.dt = dt
    self.window_sec = window_sec
    self.min_recovery_buffer_sec = min_recovery_buffer_sec
    self.min_vego = min_vego
    self.min_r2 = min_r2
    self.min_sharpness = min_sharpness
    self.min_valid_block_count = min_valid_block_count
    self.initial_lag = float(CP.longitudinalActuatorDelay)
    self.enabled = bool(CP.openpilotLongitudinalControl)

    self.t = 0.0
    self.v_t = 0.0
    self.long_active = False
    self.long_control_state = LongCtrlState.off
    self.accel_cmd = 0.0
    self.v_ego = 0.0
    self.v_ego_raw = 0.0
    self.gas_pressed = False
    self.brake_pressed = False
    self.standstill = False
    self.blend_ok = True

    self.last_inactive_t = 0.0
    self.last_pedal_t = 0.0
    self.last_blend_t = 0.0
    self.last_invalid_t = 0.0
    self.last_estimate_t = {r.name: 0.0 for r in REGIMES}
    self.last_fit: SplitFit | None = None

    self.points = Points(int(self.window_sec / self.dt))
    self.block_avg = {r.name: BlockAverage(BLOCK_NUM, r.block_size, 0, self.initial_lag) for r in REGIMES}

  def reset(self, initial: dict[str, tuple[float, int]]):
    """Restore regimes from a previous drive: {name: (lag, valid_blocks)}."""
    for regime in REGIMES:
      if regime.name in initial:
        lag, valid_blocks = initial[regime.name]
        self.block_avg[regime.name] = BlockAverage(BLOCK_NUM, regime.block_size, valid_blocks, lag)

  def handle_log(self, t: float, which: str, msg: capnp._DynamicStructReader):
    if which == "carControl":
      self.long_active = msg.longActive
      self.long_control_state = msg.actuators.longControlState
      self.accel_cmd = msg.actuators.accel
    elif which == "carState":
      self.v_t = t
      self.v_ego = msg.vEgo
      self.v_ego_raw = msg.vEgoRaw
      self.gas_pressed = msg.gasPressed
      self.brake_pressed = msg.brakePressed
      self.standstill = msg.standstill
    elif which == "starpilotCarControl":
      # blended ACC mixes in the stock command, which isn't logged; only learn from what openpilot alone sent
      info = msg.blendedAccInfo
      self.blend_ok = not info.valid or info.blendFactor >= 0.99
    self.t = t

  def update_points(self):
    active = self.long_active and self.long_control_state == LongCtrlState.pid and not self.standstill
    pedal = self.gas_pressed or self.brake_pressed
    sane = abs(self.accel_cmd) < MAX_ABS_ACCEL and np.isfinite(self.v_ego_raw) and self.v_ego > self.min_vego

    if not active:
      self.last_inactive_t = self.t
    if pedal:
      self.last_pedal_t = self.t
    if not self.blend_ok:
      self.last_blend_t = self.t
    if not sane:
      self.last_invalid_t = self.t

    recovered = all(self.t - last_t >= self.min_recovery_buffer_sec
                    for last_t in (self.last_inactive_t, self.last_pedal_t, self.last_blend_t, self.last_invalid_t))
    okay = self.enabled and active and not pedal and sane and self.blend_ok and recovered

    # stamped with carState's own time, since the accel is differentiated from its speed
    self.points.update(self.v_t, self.accel_cmd, self.v_ego_raw, brake_threshold(self.v_ego), okay)

  def update_estimate(self):
    if self.points.num_points < 3:
      return

    times, cmd, v, thr, okay = self.points.get()
    accel, usable = true_accel(times, v, okay)
    fit = fit_split_delay(cmd, accel, thr, usable, self.dt)
    self.last_fit = fit
    if fit is None or fit.r2 < self.min_r2:
      return

    margin = cmd - thr
    for regime in REGIMES:
      result = fit.regimes[regime.name]
      if result.seconds < regime.min_okay_sec or result.cmd_range < regime.min_cmd_range:
        continue
      if result.sharpness < self.min_sharpness or not (MIN_GAIN <= result.gain <= MAX_GAIN):
        continue
      if not (MIN_LONG_DELAY <= result.delay < MAX_LAG):
        continue
      # only estimate again once this regime has seen new data
      last_t = self.last_estimate_t[regime.name]
      if last_t != 0 and not np.any(usable[times > last_t] & (margin[times > last_t] * regime.sign > REGIME_DEADBAND)):
        continue

      self.block_avg[regime.name].update(result.delay)
      self.last_estimate_t[regime.name] = self.t

  def _fill_regime(self, out, regime: Regime, debug: bool):
    block_avg = self.block_avg[regime.name]
    valid_mean, valid_std, current_mean, current_std = block_avg.get()

    if block_avg.valid_blocks >= self.min_valid_block_count and not np.isnan(valid_mean) and not np.isnan(valid_std):
      out.status = LongDelayStatus.invalid if valid_std > MAX_LAG_STD else LongDelayStatus.estimated
    else:
      out.status = LongDelayStatus.unestimated

    out.delay = valid_mean if out.status == LongDelayStatus.estimated else self.initial_lag
    if not np.isnan(current_mean) and not np.isnan(current_std):
      out.delayEstimate = current_mean
      out.delayEstimateStd = current_std
    else:
      out.delayEstimate = self.initial_lag
      out.delayEstimateStd = 0.0

    out.validBlocks = block_avg.valid_blocks
    out.calPerc = min(100 * (block_avg.valid_blocks * block_avg.block_size + block_avg.idx) //
                      (self.min_valid_block_count * block_avg.block_size), 100)
    if debug:
      out.points = block_avg.values.flatten().tolist()

  def get_msg(self, valid: bool, debug: bool = False) -> capnp._DynamicStructBuilder:
    msg = messaging.new_message('starpilotLongitudinalDelay')
    msg.valid = valid

    long_delay = msg.starpilotLongitudinalDelay
    for regime in REGIMES:
      self._fill_regime(getattr(long_delay, regime.name), regime, debug)
    long_delay.brakeThreshold = brake_threshold(self.v_ego)
    return msg


def retrieve_initial_lag(params: Params, CP: car.CarParams) -> dict[str, tuple[float, int]] | None:
  last_lag_data = params.get("LiveLongitudinalDelay")
  last_carparams_data = params.get("CarParamsPrevRoute")

  if last_lag_data is not None:
    try:
      with log.Event.from_bytes(last_lag_data) as last_lag_msg, car.CarParams.from_bytes(last_carparams_data) as last_CP:
        if last_CP.carFingerprint != CP.carFingerprint:
          raise Exception("Car model mismatch")

        initial = {}
        for regime in REGIMES:
          saved = getattr(last_lag_msg.starpilotLongitudinalDelay, regime.name)
          assert saved.validBlocks <= BLOCK_NUM, "Invalid number of valid blocks"
          # a regime whose blocks disagreed starts over; the other keeps what it learned
          if saved.status != LongDelayStatus.invalid:
            initial[regime.name] = (saved.delayEstimate, saved.validBlocks)
        return initial
    except Exception as e:
      cloudlog.error(f"Failed to retrieve initial longitudinal lag: {e}")
      params.remove("LiveLongitudinalDelay")

  return None


def main():
  config_realtime_process([0, 1, 2, 3], 5)

  DEBUG = bool(int(os.getenv("DEBUG", "0")))

  pm = messaging.PubMaster(['starpilotLongitudinalDelay'])
  # carState and carControl run at 100Hz but are conflated; livePose clocks the 20Hz points, as in lagd
  sm = messaging.SubMaster(['livePose', 'carState', 'carControl', 'starpilotCarControl'], poll='livePose')

  params = Params()
  CP = messaging.log_from_bytes(params.get("CarParams", block=True), car.CarParams)

  lag_learner = LongitudinalLagEstimator(CP, 1. / SERVICE_LIST['livePose'].frequency)
  if (initial := retrieve_initial_lag(params, CP)) is not None:
    lag_learner.reset(initial)

  while True:
    sm.update()
    valid = sm.all_checks(['carState', 'carControl'])
    if valid:
      for which in sorted(sm.updated.keys(), key=lambda x: sm.logMonoTime[x]):
        if sm.updated[which]:
          lag_learner.handle_log(sm.logMonoTime[which] * 1e-9, which, sm[which])
      if sm.updated['carState']:
        lag_learner.update_points()

    # 4Hz driven by livePose
    if sm.frame % 5 == 0:
      lag_learner.update_estimate()
      lag_msg_dat = lag_learner.get_msg(valid, DEBUG).to_bytes()
      pm.send('starpilotLongitudinalDelay', lag_msg_dat)

      if sm.frame % 1200 == 0:  # cache every 60 seconds
        params.put_nonblocking("LiveLongitudinalDelay", lag_msg_dat)


if __name__ == "__main__":
  main()
