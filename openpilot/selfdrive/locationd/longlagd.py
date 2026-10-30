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

The command is carOutput's accel, what the car was actually sent: on GEN2 Mazda with blended ACC
that includes the stock MRCC share, so blended driving is data too. The response is a zero-phase
derivative of vEgoRaw. Each estimate is a batch over the buffered window, so a non-causal filter
costs nothing, and it keeps the filter lag in aEgo (a Kalman filter) and deviceMotion (an EKF) out
of the measured delay.

Braking is a few percent of openpilot-only driving, so the brake delay is built up over many drives:
every block's mean and the partly filled block are saved (also on shutdown) and picked up by the next
drive, and a delay is trusted once the standard error of its block means is small, however much the
blocks scatter. Windows are taken at a modest r2 because the plant also lags, which a pure-delay
model can't explain.

log.capnp is pinned, so there is no event for this: the state is saved as JSON in
LiveLongitudinalDelay (every 60 s and on shutdown), which is what plannerd reads, and the same
numbers go to the log as a "longlagd" event.
"""
import os
from collections import deque
from dataclasses import dataclass

import capnp
import numpy as np

import openpilot.cereal.messaging as messaging
from opendbc.car.structs import car
from openpilot.cereal.services import SERVICE_LIST
from openpilot.common.params import Params
from openpilot.common.realtime import config_realtime_process
from openpilot.common.swaglog import cloudlog
from openpilot.selfdrive.locationd.helpers import parabolic_peak_interp
from openpilot.selfdrive.locationd.lagd import BlockAverage, masked_symmetric_moving_average
from openpilot.starpilot.longitudinal.learned_delay import MIN_LONG_DELAY, MAX_LONG_DELAY, PARAM, VERSION, brake_threshold, \
                                                         saved_state_matches

LongCtrlState = car.CarControl.Actuators.LongControlState

BLOCK_NUM = 50
BLOCK_NUM_NEEDED = 5
WINDOW_SEC = 120.0
MIN_RECOVERY_BUFFER_SEC = 2.0
MIN_VEGO = 5.0
MAX_ABS_ACCEL = 4.0
MAX_LAG = MAX_LONG_DELAY
# Block means scattered more than this (std, s) aren't one delay at all.
MAX_LAG_STD = 0.25
# A delay is trusted once its mean is pinned down this well (standard error of the block means, s).
# The scatter between blocks can be wide: braking is seen a few events at a time, and the answer
# still firms up as more of them come in. Judging the scatter itself (lagd's way, for a steady
# lateral delay) kept the brake regime invalid on real driving.
MAX_LAG_SE = 0.04
# Share of the (band-passed) accel the two-path model has to explain. The CX-30's real windows
# reach a median of 0.66 with a pure-delay model (the plant also lags), so 0.7 threw most of them away.
MIN_R2 = 0.5
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


# Braking is a small share of driving (a few percent on the CX-30, at the -0.75 threshold), so the brake
# regime is allowed to estimate from much less data: on sparse-braking drives 2.5s learns about 1.5x
# faster than 4s with the same delay, and the sharpness test and the block scatter still catch windows
# that say nothing.
REGIMES = (
  Regime("gas", 1, block_size=100, min_okay_sec=20.0, min_cmd_range=0.3),
  Regime("brake", -1, block_size=40, min_okay_sec=2.5, min_cmd_range=0.3),
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


@dataclass
class SavedRegime:
  """What a regime carries from one drive to the next. The block means and the partly filled
  block come with it when the drive saved them; older saves only have an average to restart from."""
  lag: float
  valid_blocks: int
  values: list[float] | None = None
  block_idx: int = 0
  fill: int = 0


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
  inputs = {"carControl", "carState", "carOutput"}

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
    # a car with stock cruise only follows the command while its own cruise is engaged
    self.pcm_cruise = bool(CP.pcmCruise)

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
    self.cruise_engaged = False

    self.last_inactive_t = 0.0
    self.last_pedal_t = 0.0
    self.last_invalid_t = 0.0
    self.last_estimate_t = {r.name: 0.0 for r in REGIMES}
    self.last_fit: SplitFit | None = None
    # why each regime's last estimate was or wasn't taken, for tuning the thresholds from replays
    self.last_outcome: dict[str, str] = {r.name: "no fit" for r in REGIMES}

    self.points = Points(int(self.window_sec / self.dt))
    self.block_avg = {r.name: BlockAverage(BLOCK_NUM, r.block_size, 0, self.initial_lag) for r in REGIMES}

  def reset(self, initial: dict[str, SavedRegime | tuple[float, int]]):
    """Restore regimes from a previous drive."""
    for regime in REGIMES:
      if regime.name not in initial:
        continue
      saved = initial[regime.name]
      if not isinstance(saved, SavedRegime):
        saved = SavedRegime(*saved)
      block_avg = BlockAverage(BLOCK_NUM, regime.block_size, saved.valid_blocks, saved.lag)

      values = None if saved.values is None else np.asarray(saved.values, dtype=np.float64)
      # the saved blocks have to agree with the count they came with: before the buffer wraps the
      # filling block is the one after the valid ones, and it is always one of the blocks
      consistent = (values is not None and values.shape == (BLOCK_NUM,) and np.all(np.isfinite(values)) and
                    0 <= saved.block_idx < BLOCK_NUM and 0 <= saved.fill < regime.block_size and
                    (saved.valid_blocks >= BLOCK_NUM or saved.block_idx == saved.valid_blocks))
      if consistent:
        block_avg.values = values.reshape(BLOCK_NUM, 1)
        block_avg.block_idx = saved.block_idx
        block_avg.idx = saved.fill
      self.block_avg[regime.name] = block_avg

  def handle_log(self, t: float, which: str, msg: capnp._DynamicStructReader):
    if which == "carControl":
      self.long_active = msg.longActive
      # .raw, a plain int: the enum itself keeps the whole message it was read from alive
      self.long_control_state = msg.actuators.longControlState.raw
    elif which == "carOutput":
      self.accel_cmd = msg.actuatorsOutput.accel
    elif which == "carState":
      self.v_t = t
      self.v_ego = msg.vEgo
      self.v_ego_raw = msg.vEgoRaw
      self.gas_pressed = msg.gasPressed
      self.brake_pressed = msg.brakePressed
      self.standstill = msg.standstill
      self.cruise_engaged = msg.cruiseState.enabled
    self.t = t

  def update_points(self):
    cruise_ok = self.cruise_engaged or not self.pcm_cruise
    active = self.long_active and self.long_control_state == LongCtrlState.pid and not self.standstill and cruise_ok
    pedal = self.gas_pressed or self.brake_pressed
    sane = abs(self.accel_cmd) < MAX_ABS_ACCEL and np.isfinite(self.v_ego_raw) and self.v_ego > self.min_vego

    if not active:
      self.last_inactive_t = self.t
    if pedal:
      self.last_pedal_t = self.t
    if not sane:
      self.last_invalid_t = self.t

    recovered = all(self.t - last_t >= self.min_recovery_buffer_sec
                    for last_t in (self.last_inactive_t, self.last_pedal_t, self.last_invalid_t))
    okay = self.enabled and active and not pedal and sane and recovered

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
      self.last_outcome = dict.fromkeys(self.last_outcome, "no fit" if fit is None else "fit r2")
      return

    margin = cmd - thr
    for regime in REGIMES:
      self.last_outcome[regime.name] = self._judge(regime, fit.regimes[regime.name], times, usable, margin)
      if self.last_outcome[regime.name] == "accepted":
        self.block_avg[regime.name].update(fit.regimes[regime.name].delay)
        self.last_estimate_t[regime.name] = self.t

  def _judge(self, regime: Regime, result: RegimeFit, times: np.ndarray, usable: np.ndarray, margin: np.ndarray) -> str:
    if result.seconds < regime.min_okay_sec:
      return "too little data"
    if result.cmd_range < regime.min_cmd_range:
      return "command range"
    if result.sharpness < self.min_sharpness:
      return "sharpness"
    if not (MIN_GAIN <= result.gain <= MAX_GAIN):
      return "gain"
    if not (MIN_LONG_DELAY <= result.delay < MAX_LAG):
      return "delay bounds"
    # only estimate again once this regime has seen new data
    last_t = self.last_estimate_t[regime.name]
    if last_t != 0 and not np.any(usable[times > last_t] & (margin[times > last_t] * regime.sign > REGIME_DEADBAND)):
      return "no new data"
    return "accepted"

  def _regime_state(self, regime: Regime, with_blocks: bool) -> dict:
    block_avg = self.block_avg[regime.name]
    valid_mean, valid_std, current_mean, current_std = block_avg.get()
    # get() leaves out the block that is filling, which is one of the valid ones once the buffer wraps
    n_valid = block_avg.valid_blocks - (1 if block_avg.valid_blocks >= block_avg.num_blocks else 0)
    std_err = valid_std / np.sqrt(n_valid) if n_valid > 0 else np.nan

    status = 'unestimated'
    if n_valid >= self.min_valid_block_count and np.isfinite(valid_mean) and np.isfinite(valid_std):
      if valid_std > MAX_LAG_STD:
        status = 'invalid'
      elif std_err <= MAX_LAG_SE:
        status = 'estimated'

    if not np.isnan(current_mean) and not np.isnan(current_std):
      estimate, estimate_std = current_mean, current_std
    else:
      estimate, estimate_std = self.initial_lag, 0.0

    # progress to "estimated": enough blocks, and then a mean pinned down well enough
    progress = ((block_avg.valid_blocks * block_avg.block_size + block_avg.idx) /
                (self.min_valid_block_count * block_avg.block_size))
    if n_valid >= self.min_valid_block_count and np.isfinite(std_err) and std_err > 0:
      progress = min(progress, MAX_LAG_SE / std_err)

    state = {
      'delay': float(valid_mean if status == 'estimated' else self.initial_lag),
      'status': status,
      'delayEstimate': float(estimate),
      'delayEstimateStd': float(estimate_std),
      'validBlocks': int(block_avg.valid_blocks),
      'calPerc': 100 if status == 'estimated' else min(int(100 * progress), 99),
      'blockIndex': int(block_avg.block_idx),
      'blockFill': int(block_avg.idx),
    }
    if with_blocks:
      state['points'] = [float(x) for x in block_avg.values.flatten()]
    return state

  def get_state(self, CP: car.CarParams, persist: bool = False) -> dict:
    """The learner's state. `persist` adds every block's mean, which is what a drive saves so the next
    one can carry on from the same state; it is left out of the logged copy to keep the logs small."""
    state = {regime.name: self._regime_state(regime, persist) for regime in REGIMES}
    state.update(version=VERSION, carFingerprint=CP.carFingerprint, brakeThreshold=brake_threshold(self.v_ego))
    return state


def initial_from_saved(saved: dict) -> dict[str, SavedRegime]:
  """What a drive starts from, given the last one's saved state (get_state(persist=True))."""
  initial = {}
  for regime in REGIMES:
    regime_state = saved[regime.name]
    assert 0 <= regime_state['validBlocks'] <= BLOCK_NUM, "Invalid number of valid blocks"
    # a regime whose blocks disagreed wildly starts over; the other keeps what it learned
    if regime_state['status'] != 'invalid':
      initial[regime.name] = SavedRegime(float(regime_state['delayEstimate']), int(regime_state['validBlocks']),
                                         regime_state.get('points') or None,
                                         int(regime_state.get('blockIndex', 0)), int(regime_state.get('blockFill', 0)))
  return initial


def retrieve_initial_lag(params: Params, CP: car.CarParams) -> dict[str, SavedRegime] | None:
  saved = params.get(PARAM)
  if saved is not None:
    try:
      if not saved_state_matches(CP, saved):
        raise Exception("Saved for another car or version")
      return initial_from_saved(saved)
    except Exception as e:
      cloudlog.error(f"Failed to retrieve initial longitudinal lag: {e}")
      params.remove(PARAM)

  return None


def main():
  config_realtime_process([0, 1, 2, 3], 5)

  DEBUG = bool(int(os.getenv("DEBUG", "0")))

  # carState, carControl and carOutput run at 100Hz but are conflated; deviceMotion clocks the 20Hz points, as in lagd
  sm = messaging.SubMaster(['deviceMotion', 'carState', 'carControl', 'carOutput'], poll='deviceMotion')

  params = Params()
  CP = messaging.log_from_bytes(params.get("CarParams", block=True), car.CarParams)

  lag_learner = LongitudinalLagEstimator(CP, 1. / SERVICE_LIST['deviceMotion'].frequency)
  if (initial := retrieve_initial_lag(params, CP)) is not None:
    lag_learner.reset(initial)

  try:
    while True:
      sm.update()
      if sm.all_checks(['carState', 'carControl', 'carOutput']):
        for which in sorted(sm.updated.keys(), key=lambda x: sm.logMonoTime[x]):
          if sm.updated[which]:
            lag_learner.handle_log(sm.logMonoTime[which] * 1e-9, which, sm[which])
        if sm.updated['carState']:
          lag_learner.update_points()

      # 4Hz driven by deviceMotion
      if sm.frame % 5 == 0:
        lag_learner.update_estimate()

        if sm.frame % 1200 == 0:  # save every 60 seconds
          state = lag_learner.get_state(CP, persist=True)
          params.put(PARAM, state)
          logged = state if DEBUG else lag_learner.get_state(CP)
          cloudlog.event("longlagd", **logged, outcome=dict(lag_learner.last_outcome))
  finally:
    # braking is a minute or so of a drive, so don't lose the end of it to the shutdown
    if lag_learner.points.num_points > 0:
      params.put(PARAM, lag_learner.get_state(CP, persist=True), block=True)


if __name__ == "__main__":
  main()
