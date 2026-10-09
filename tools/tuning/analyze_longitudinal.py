#!/usr/bin/env python3
"""Analyze a route for longitudinal tuning opportunities.

Answers the three questions a longitudinal tune actually turns on:

  1. Is the car's accel interface linear, and is its gain 1.0? If it is not, the
     command needs a static map the way lateral needs siglin. If it is, no map is
     worth adding and the gains are the whole story.
  2. How fast is the plant? A deadtime and a first-order lag are fitted from the
     command/response pair; those set what feedback gains are safe.
  3. Where does the tracking error live -- measurement noise, phase lag, or a slow
     standing offset the integrator is too weak to cancel?
  4. Does the delay differ between requests the powertrain meets and ones that need the
     brakes, and with speed? This fits longlagd's two-path model over the whole route and per
     speed band, sweeps the brake threshold to suggest a table for
     starpilot/common/longitudinal_delay.py, and times individual steps without any model.
     --replay-learner runs longlagd itself over the route, to see how fast it converges.

Usage:
  ./analyze_longitudinal.py <dongle>/<route>
  ./analyze_longitudinal.py <dongle>/<route> --mode rlog
  ./analyze_longitudinal.py <dongle>/<route> --replay-learner
"""
import argparse
import math
from dataclasses import dataclass

import numpy as np

from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.locationd import longlagd
from openpilot.selfdrive.locationd.lagd import LateralLagEstimator
from openpilot.starpilot.common.longitudinal_delay import BRAKE_THRESHOLD_BP, brake_threshold
from openpilot.tools.lib.logreader import LogReader, ReadMode

# The car has to hold a near-constant command for this long before the sample counts
# as steady state, otherwise the plant's own lag pollutes the gain estimate.
QSS_WINDOW_S = 1.0
QSS_MAX_COMMAND_SPREAD = 0.15
# Real vehicle acceleration does not live above a couple of Hz; anything faster is
# noise in aEgo, which is a differentiated wheel speed.
NOISE_CUTOFF_HZ = 1.5

LongCtrlStatePid = 1

# The delay analysis runs at longlagd's 20Hz, which also keeps its lag grid small.
DELAY_DECIMATION = 5
DELAY_DT = DT_CTRL * DELAY_DECIMATION
SPEED_BANDS = (("5-15 m/s", 5.0, 15.0), ("15-25 m/s", 15.0, 25.0), ("25+ m/s", 25.0, math.inf))
THRESHOLD_OFFSETS = np.round(np.arange(-0.3, 0.301, 0.05), 2)


@dataclass
class Sample:
  t: float
  v_ego: float
  a_ego: float
  a_target: float
  accel_cmd: float
  long_active: bool
  gas_pressed: bool
  brake_pressed: bool
  long_control_state: int
  has_lead: bool
  v_ego_raw: float = 0.0
  blend_ok: bool = True
  brake_pressure: float = math.nan


def _resample(samples: list[Sample]) -> dict[str, np.ndarray]:
  """Put every signal on a uniform DT_CTRL grid so the fits below are well posed."""
  t = np.array([s.t for s in samples])
  grid = np.arange(t[0], t[-1], DT_CTRL)
  out = {"t": grid}
  for name in ("v_ego", "a_ego", "a_target", "accel_cmd", "v_ego_raw", "brake_pressure"):
    out[name] = np.interp(grid, t, np.array([getattr(s, name) for s in samples]))
  for name in ("long_active", "gas_pressed", "brake_pressed", "long_control_state", "has_lead", "blend_ok"):
    raw = np.array([float(getattr(s, name)) for s in samples])
    out[name] = raw[np.clip(np.searchsorted(t, grid, side="right") - 1, 0, len(t) - 1)]
  return out


def _engaged_runs(d: dict[str, np.ndarray], min_seconds: float = 3.0):
  """Index pairs where openpilot longitudinal was driving, uninterrupted."""
  m = (d["long_active"] > 0.5) & (d["gas_pressed"] < 0.5) & (d["brake_pressed"] < 0.5)
  m &= np.abs(d["long_control_state"] - LongCtrlStatePid) < 0.1
  # with blended ACC the stock command is mixed in, and it isn't logged
  m &= d["blend_ok"] > 0.5
  edges = np.flatnonzero(np.diff(np.concatenate(([0], m.view(np.int8), [0]))))
  n = int(round(min_seconds / DT_CTRL))
  return [(a, b) for a, b in zip(edges[::2], edges[1::2], strict=True) if b - a >= n]


def _lowpass(x: np.ndarray, cutoff_hz: float) -> np.ndarray:
  from scipy.signal import butter, filtfilt
  b, a = butter(2, cutoff_hz / (0.5 / DT_CTRL), btype="low")
  return filtfilt(b, a, x)


def _simulate(cmd: np.ndarray, dead_frames: int, tau: float, a0: float) -> np.ndarray:
  alpha = DT_CTRL / (tau + DT_CTRL)
  delayed = np.concatenate((np.full(dead_frames, cmd[0]), cmd[:-dead_frames])) if dead_frames else cmd
  out = np.empty_like(cmd)
  state = a0
  for i, x in enumerate(delayed):
    state += alpha * (x - state)
    out[i] = state
  return out


def summarize_static_map(runs, d) -> None:
  """Bin achieved accel against a held command. A gain that stays near 1.0 across the
  bins means the interface is already linear and needs no siglin-style inversion."""
  lag = int(round(0.30 / DT_CTRL))
  win = int(round(QSS_WINDOW_S / DT_CTRL))
  cmd_m, ach_m = [], []
  for a, b in runs:
    i = a
    while i + win + lag < b:
      c = d["accel_cmd"][i:i + win]
      if c.max() - c.min() > QSS_MAX_COMMAND_SPREAD:
        i += int(round(0.1 / DT_CTRL))
        continue
      cmd_m.append(c.mean())
      ach_m.append(d["a_ego"][i + lag:i + lag + win].mean())
      i += int(round(0.25 / DT_CTRL))
  if len(cmd_m) < 50:
    print("\nStatic map: not enough steady-state windows.")
    return
  cmd = np.array(cmd_m)
  ach = np.array(ach_m)

  print(f"\nStatic map (command -> achieved accel), {len(cmd)} steady windows:")
  print(f"  {'command bin':>16} {'n':>6} {'cmd':>8} {'achieved':>9} {'gain':>7}")
  edges = [-3.0, -1.5, -1.0, -0.6, -0.35, -0.15, -0.05, 0.05, 0.15, 0.35, 0.6, 1.0, 1.5, 3.0]
  for lo, hi in zip(edges[:-1], edges[1:], strict=True):
    m = (cmd >= lo) & (cmd < hi)
    if m.sum() < 25:
      continue
    mc, ma = cmd[m].mean(), ach[m].mean()
    # the ratio only means anything away from zero, where a small offset is not a large gain
    gain = ma / mc if abs(mc) > 0.20 else float("nan")
    print(f"  [{lo:+5.2f},{hi:+5.2f}) {m.sum():6d} {mc:+8.3f} {ma:+9.3f} {gain:7.2f}")

  ones = np.ones_like(cmd)
  linear = np.linalg.lstsq(np.column_stack([cmd, ones]), ach, rcond=None)[0]
  linear_res = float((ach - np.column_stack([cmd, ones]) @ linear).std())
  print(f"  linear fit: achieved = {linear[0]:.3f}*cmd {linear[1]:+.3f}   residual {linear_res:.4f}")

  # Does letting the curve bend explain anything the straight line cannot? Separate
  # brake/throttle slopes plus a quadratic term will catch a siglin shape if one is there.
  design = np.column_stack([cmd, np.clip(cmd, 0.0, None), cmd * np.abs(cmd), ones])
  bent = np.linalg.lstsq(design, ach, rcond=None)[0]
  bent_res = float((ach - design @ bent).std())
  print(f"  bent fit:   brake slope {bent[0]:.3f}, throttle slope {bent[0] + bent[1]:.3f}, " +
        f"curvature {bent[2]:+.3f}   residual {bent_res:.4f}")

  improvement = 1.0 - bent_res / linear_res if linear_res > 0 else 0.0
  if improvement < 0.05 and abs(linear[0] - 1.0) < 0.15:
    print(f"  -> linear and unity-gain (bending the curve buys only {100 * improvement:.1f}%): " +
          "a static command map would buy nothing, tune the gains instead.")
  else:
    print(f"  -> bending the curve buys {100 * improvement:.1f}% and the slope is " +
          f"{linear[0]:.2f}: a static map (siglin-style) is worth fitting.")


def summarize_plant(runs, d) -> None:
  """Fit deadtime + first-order lag. These set the achievable closed-loop bandwidth."""
  best = None
  for dead in range(0, 36, 3):
    for tau in (0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.7):
      res = np.concatenate([
        d["a_ego"][a:b] - _simulate(d["accel_cmd"][a:b], dead, tau, d["a_ego"][a]) for a, b in runs
      ])
      if best is None or res.std() < best[0]:
        best = (res.std(), dead, tau)
  err, dead, tau = best
  print("\nPlant (command -> achieved accel):")
  print(f"  deadtime {dead * DT_CTRL * 1000:.0f} ms, tau {tau:.2f} s, residual {err:.4f} m/s^2")
  print(f"  63% response in ~{dead * DT_CTRL + tau:.2f} s")
  print(f"  IMC-PI at lambda=tau would be kp~{tau / (tau + dead * DT_CTRL):.2f}, " +
        f"ki~{1.0 / (tau + dead * DT_CTRL):.2f}")


def summarize_tracking(runs, d) -> None:
  """Split the tracking error into noise, phase lag and slow standing offset. A big
  slow component with a weak ki is the signature of an integrator that is too slow."""
  raw = np.concatenate([(d["a_ego"] - d["a_target"])[a:b] for a, b in runs])
  real = np.concatenate([_lowpass((d["a_ego"] - d["a_target"])[a:b], NOISE_CUTOFF_HZ) for a, b in runs])
  print("\nTracking error (achieved - target):")
  print(f"  raw std           {raw.std():.4f} m/s^2  bias {raw.mean():+.4f}")
  print(f"  real (<{NOISE_CUTOFF_HZ} Hz)     {real.std():.4f} m/s^2")
  print(f"  measurement noise {np.sqrt(max(raw.std() ** 2 - real.std() ** 2, 0.0)):.4f} m/s^2")

  v = np.concatenate([d["v_ego"][a:b] for a, b in runs])
  print("  by speed:")
  for label, m in (("<10 mph", v < 4.5), ("10-30", (v >= 4.5) & (v < 13.4)),
                   ("30-50", (v >= 13.4) & (v < 22.4)), ("50+", v >= 22.4)):
    if m.sum() > int(round(5.0 / DT_CTRL)):
      print(f"    {label:8s} n={m.sum() * DT_CTRL / 60:5.1f} min  real std {real[m].std():.4f}")

  best = None
  for shift in range(0, 80, 2):
    e = np.concatenate([
      (_lowpass(d["a_ego"][a:b], NOISE_CUTOFF_HZ)[shift:] -
       _lowpass(d["a_target"][a:b], NOISE_CUTOFF_HZ)[:len(range(a, b)) - shift]) if shift else
      (_lowpass(d["a_ego"][a:b], NOISE_CUTOFF_HZ) - _lowpass(d["a_target"][a:b], NOISE_CUTOFF_HZ))
      for a, b in runs if b - a > shift + 50
    ])
    if best is None or e.std() < best[1]:
      best = (shift, e.std())
  print(f"  best target shift {best[0] * DT_CTRL * 1000:.0f} ms -> {best[1]:.4f} " +
        f"({100 * (1 - best[1] / real.std()):.0f}% of the real error is pure lag)")

  from scipy.signal import welch
  f, p = welch(real, fs=1.0 / DT_CTRL, nperseg=min(2048, len(real)))
  print("  error power by band:")
  for lo, hi in ((0.05, 0.3), (0.3, 0.8), (0.8, 1.5)):
    m = (f >= lo) & (f < hi)
    print(f"    {lo:4.2f}-{hi:<4.1f} Hz {100 * p[m].sum() / p.sum():5.1f}%")
  slow = (f >= 0.05) & (f < 0.3)
  if p[slow].sum() / p.sum() > 0.3:
    print("  -> error is dominated by a slow standing offset: ki is too weak to cancel it.")



def _delay_series(runs, d) -> dict[str, np.ndarray]:
  """The route on longlagd's 20Hz grid, with the samples it would learn from marked usable."""
  engaged = np.zeros(len(d["t"]), dtype=bool)
  for a, b in runs:
    engaged[a:b] = True
  idx = np.arange(0, len(d["t"]), DELAY_DECIMATION)
  t, v = d["t"][idx], d["v_ego"][idx]
  accel, usable = longlagd.true_accel(t, d["v_ego_raw"][idx], engaged[idx] & (v > longlagd.MIN_VEGO))
  return {"t": t, "v": v, "cmd": d["accel_cmd"][idx], "a_ego": d["a_ego"][idx], "accel": accel, "usable": usable,
          "thr": np.array([brake_threshold(x) for x in v]), "pressure": d["brake_pressure"][idx]}


def _bands(s):
  for label, lo, hi in SPEED_BANDS:
    yield label, lo, hi, s["usable"] & (s["v"] >= lo) & (s["v"] < hi)


def _print_fit(label: str, fit) -> None:
  if fit is None:
    print(f"  {label:10s} not enough data")
    return
  parts = [f"{name} {r.delay:.2f}s (gain {r.gain:.2f}, sharp {r.sharpness:.4f}, {r.seconds:4.0f}s)" for name, r in fit.regimes.items()]
  print(f"  {label:10s} r2 {fit.r2:.2f}  " + "  ".join(parts))


def summarize_measurement_lag(s) -> None:
  """aEgo is a Kalman-filtered derivative, so it trails the real accel. Anything timed off it
  (including a delay tuned by eye from plots) carries that lag along."""
  usable = s["usable"]
  if np.count_nonzero(usable) < int(30 / DELAY_DT):
    return
  ref, a_ego = longlagd.band_pass(s["accel"], usable), longlagd.band_pass(s["a_ego"], usable)
  lag, corr, _ = LateralLagEstimator.actuator_delay(ref, a_ego, usable, DELAY_DT, 1.0)
  print(f"\naEgo trails the zero-phase wheel-speed accel by {1000 * lag:.0f} ms (corr {corr:.2f}).")


def summarize_delay_split(s) -> None:
  """Fit longlagd's two-path model: the powertrain answers down to the brake threshold after one
  delay, the brakes the rest after another. Delays here include the plant's lag, so they are the
  lookaheads the planner wants rather than pure deadtime."""
  print("\nDelay by regime (longlagd's two-path fit):")
  fit = longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"], s["usable"], DELAY_DT)
  _print_fit("all", fit)
  for label, _, _, band in _bands(s):
    _print_fit(label, longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"], band, DELAY_DT))
  print(f"  a delay is only pinned down with sharpness >= {longlagd.MIN_SHARPNESS} and some tens of seconds in its regime")
  if fit is not None:
    gap = fit.regimes["brake"].delay - fit.regimes["gas"].delay
    print(f"  -> the brakes answer {1000 * abs(gap):.0f} ms {'later' if gap > 0 else 'sooner'} than the powertrain")


def summarize_brake_threshold(s) -> None:
  """Move the assumed brake threshold and see where the two-path model fits best. The threshold
  is where requests stop being met by closing the throttle, so it should get more negative
  with speed as drag and engine braking grow."""
  print("\nBrake threshold sweep (r2 of the two-path fit, offset from the current table):")
  bands = list(_bands(s))
  print(f"  {'offset':>7} " + " ".join(f"{label:>10}" for label, *_ in bands))
  best: dict[str, tuple[float, float]] = {}
  for offset in THRESHOLD_OFFSETS:
    row = []
    for label, _, _, band in bands:
      fit = longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"] + offset, band, DELAY_DT)
      row.append(f"{fit.r2:10.3f}" if fit is not None else f"{'-':>10}")
      if fit is not None and fit.r2 > best.get(label, (-math.inf, 0.0))[0]:
        best[label] = (fit.r2, float(offset))
    print(f"  {offset:+7.2f} " + " ".join(row))

  if best:
    centers, values = [], []
    for label, lo, hi, _ in bands:
      if label in best:
        center = (lo + min(hi, 35.0)) / 2
        centers.append(center)
        values.append(round(brake_threshold(center) + best[label][1], 2))
    print(f"  current: BRAKE_THRESHOLD_BP = {BRAKE_THRESHOLD_BP}")
    print(f"  suggest: BRAKE_THRESHOLD_BP = {centers}, BRAKE_THRESHOLD_V = {values}")


def _half_time(x: np.ndarray, x0: float, step: float) -> float | None:
  """Sub-sample index where x first gets halfway from x0 through step, if it does."""
  frac = (x - x0) / step
  k = int(np.argmax(frac >= 0.5))
  if frac[k] < 0.5:
    return None
  if k == 0:
    return 0.0
  return k - 1 + (0.5 - frac[k - 1]) / max(frac[k] - frac[k - 1], 1e-6)


def summarize_step_events(s) -> None:
  """Time clean command steps to the wheels' response with no model in between: from the
  command's halfway point to the response's, split by which way the step went and whether it
  starts or ends past the brake threshold."""
  dt = DELAY_DT
  cmd, usable, thr = s["cmd"], s["usable"], s["thr"]
  accel = np.nan_to_num(longlagd.masked_symmetric_moving_average(s["accel"], usable, longlagd.SMOOTH_K, longlagd.SMOOTH_SIGMA))
  pre, rise, settle = int(round(0.5 / dt)), int(round(0.6 / dt)), int(round(1.5 / dt))

  events: dict[tuple[str, str], list[float]] = {}
  i = pre
  while i + rise + settle < len(cmd):
    span = slice(i - pre, i + rise + settle)
    c0 = cmd[i - pre:i]
    step = cmd[i + rise] - c0.mean()
    if not usable[span].all() or np.ptp(c0) > 0.1 or abs(step) < 0.4:
      i += 1
      continue

    t_cmd = _half_time(cmd[i:i + rise + settle], c0.mean(), step)
    t_acc = _half_time(accel[i:i + rise + settle], accel[i - pre:i].mean(), step)
    if t_cmd is not None and t_acc is not None:
      start, end, here = c0.mean(), cmd[i + rise], thr[i]
      if step > 0:
        kind = "gas apply" if start >= here else "brake release"
      elif end >= here:
        kind = "gas release"
      else:
        # a step from above the threshold is met by the powertrain first and the brakes after
        kind = "brake apply" if start < here else "into brakes"
      band = next((label for label, lo, hi in SPEED_BANDS if lo <= s["v"][i] < hi), None)
      if band is not None:
        events.setdefault((kind, band), []).append((t_acc - t_cmd) * dt)
    i += rise + settle

  print("\nStep response, command halfway -> response halfway (median [p25, p75] n):")
  print(f"  {'':14s}" + "".join(f"{label:>24}" for label, *_ in SPEED_BANDS))
  for kind in ("gas apply", "gas release", "into brakes", "brake apply", "brake release"):
    cells = []
    for label, *_ in SPEED_BANDS:
      x = events.get((kind, label), [])
      cells.append(f"{np.median(x):.2f} [{np.percentile(x, 25):.2f},{np.percentile(x, 75):.2f}] {len(x):3d}" if len(x) >= 3 else f"{len(x):>24d}")
    print(f"  {kind:14s}" + "".join(f"{c:>24}" for c in cells))


def _brake_pressure_probe(CP):
  """Decoder for the signal mazda_2019.dbc calls STEER_TORQUE_VB.BRAKE_PREASURE, which may be the
  brake pressure. If it is, it times the brakes directly and shows where they take over."""
  if CP.brand != "mazda":
    return None
  from opendbc.can.dbc import DBC as CanDBC
  from opendbc.can.parser import get_raw_value
  from opendbc.car import Bus
  from opendbc.car.mazda.values import DBC

  dbc_name = DBC.get(CP.carFingerprint, {}).get(Bus.pt)
  msg = CanDBC(dbc_name).name_to_msg.get("STEER_TORQUE_VB") if dbc_name else None
  if msg is None or "BRAKE_PREASURE" not in msg.sigs:
    return None
  sig = msg.sigs["BRAKE_PREASURE"]

  def decode(can_msgs, last: float) -> float:
    for c in can_msgs:
      if c.address == msg.address and c.src < 128:
        return get_raw_value(c.dat, sig) * sig.factor + sig.offset
    return last
  return decode


def summarize_brake_pressure(s) -> None:
  pressure = s["pressure"]
  usable = s["usable"] & np.isfinite(pressure)
  if np.count_nonzero(usable) < int(30 / DELAY_DT) or np.ptp(pressure[usable]) == 0:
    return
  brake_part = longlagd.band_pass(np.minimum(s["cmd"] - s["thr"], 0.0), usable)
  # whichever way up the signal is, it should follow the brake part of the request after some delay
  lag, ncc, _ = max((LateralLagEstimator.actuator_delay(brake_part.copy(), longlagd.band_pass(sign * pressure, usable),
                                                        usable, DELAY_DT, 1.2) for sign in (1.0, -1.0)), key=lambda r: r[1])
  print(f"\nSTEER_TORQUE_VB.BRAKE_PREASURE against the brake part of the request: best corr {ncc:.2f} at {1000 * lag:.0f} ms")
  if ncc < 0.6:
    print("  -> it doesn't follow ACC braking, so it isn't the brake pressure")
    return
  print(f"  -> looks like brake pressure: the brakes act {1000 * lag:.0f} ms after the request")

  # where the brakes come on: the request level, a delay earlier, at which pressure leaves idle
  idle_mask = usable & (s["cmd"] - s["thr"] > 0.3)
  if np.count_nonzero(idle_mask) < 100:
    return
  idle = np.median(pressure[idle_mask])
  spread = max(3 * np.median(np.abs(pressure[idle_mask] - idle)), 1e-6)
  on = np.abs(pressure - idle) > spread
  shift = int(round(lag / DELAY_DT))
  cmd_then = np.concatenate((np.full(shift, np.nan), s["cmd"][:len(s["cmd"]) - shift])) if shift else s["cmd"]
  edges = np.round(np.arange(-1.5, 0.51, 0.1), 2)
  for label, _, _, band in _bands(s):
    levels = []
    for lo, hi in zip(edges[:-1], edges[1:], strict=True):
      m = band & (cmd_then >= lo) & (cmd_then < hi)
      if np.count_nonzero(m) >= 20:
        levels.append(((lo + hi) / 2, float(on[m].mean())))
    crossing = next((c for c, frac in reversed(levels) if frac >= 0.5), None)
    if crossing is not None:
      print(f"  {label:10s} brakes on below about {crossing:+.2f} m/s^2 (table says {brake_threshold((band * s['v']).sum() / max(band.sum(), 1)):+.2f})")


class LearnerReplay:
  """Drives longlagd the way its main() does: carState, carControl and starpilotCarControl update
  its state, livePose clocks the points, and every fifth livePose an estimate."""

  def __init__(self, CP):
    self.estimator = longlagd.LongitudinalLagEstimator(CP, 0.05)
    self.frame = 0
    self.car_state_updated = False
    self.t0: float | None = None
    self.history: list[tuple] = []

  def feed(self, msg) -> None:
    which, t = msg.which(), msg.logMonoTime * 1e-9
    if self.t0 is None:
      self.t0 = t
    if which in longlagd.LongitudinalLagEstimator.inputs:
      self.estimator.handle_log(t, which, getattr(msg, which))
      self.car_state_updated |= which == "carState"
    elif which == "livePose":
      if self.car_state_updated:
        self.estimator.update_points()
        self.car_state_updated = False
      if self.frame % 5 == 0:
        self.estimator.update_estimate()
        ld = self.estimator.get_msg(True).starpilotLongitudinalDelay
        self.history.append((t - self.t0, ld.gas.status, ld.gas.delayEstimate, ld.gas.validBlocks,
                             ld.brake.status, ld.brake.delayEstimate, ld.brake.validBlocks))
      self.frame += 1

  def report(self) -> None:
    print("\nlonglagd replay:")
    if not self.history:
      print("  no livePose in this log, so nothing clocked the learner")
      return
    for offset, name in ((1, "gas"), (4, "brake")):
      first = next((h[0] for h in self.history if h[offset] == "estimated"), None)
      last = self.history[-1]
      when = f"estimated after {first / 60:.1f} min" if first is not None else "never estimated"
      print(f"  {name:5s} {when}, ends at {last[offset + 1]:.3f}s with {last[offset + 2]} blocks ({last[offset]})")
    print(f"  {'min':>6} {'gas':>16} {'brake':>16}")
    step = max(1, len(self.history) // 12)
    for h in self.history[::step]:
      print(f"  {h[0] / 60:6.1f} {h[2]:7.3f}s {h[3]:2d} blk {h[5]:7.3f}s {h[6]:2d} blk")


def main() -> None:
  parser = argparse.ArgumentParser(description="Analyze a route for longitudinal tuning opportunities.")
  parser.add_argument("route", help="Route name, e.g. dongle/route")
  parser.add_argument("--mode", choices=("auto", "qlog", "rlog"), default="rlog")
  parser.add_argument("--replay-learner", action="store_true", help="run longlagd over the route and report how it converges")
  args = parser.parse_args()

  mode_map = {"auto": ReadMode.AUTO, "qlog": ReadMode.QLOG, "rlog": ReadMode.RLOG}
  log_reader = LogReader(args.route, default_mode=mode_map[args.mode], sort_by_time=True)

  car_params = None
  latest: dict = {}
  samples: list[Sample] = []
  learner: LearnerReplay | None = None
  pressure_probe = None
  pressure = math.nan
  blend_ok = True
  for msg in log_reader:
    which = msg.which()
    if learner is not None:
      learner.feed(msg)
    if which == "carParams" and car_params is None:
      car_params = msg.carParams
      pressure_probe = _brake_pressure_probe(car_params)
      if args.replay_learner:
        learner = LearnerReplay(car_params)
    elif which == "can" and pressure_probe is not None:
      pressure = pressure_probe(msg.can, pressure)
    elif which == "starpilotCarControl":
      info = msg.starpilotCarControl.blendedAccInfo
      blend_ok = not info.valid or info.blendFactor >= 0.99
    elif which in ("carControl", "longitudinalPlan"):
      latest[which] = getattr(msg, which)
    elif which == "carState" and "carControl" in latest and "longitudinalPlan" in latest:
      cs, cc, lp = msg.carState, latest["carControl"], latest["longitudinalPlan"]
      samples.append(Sample(
        t=msg.logMonoTime * 1e-9, v_ego=cs.vEgo, a_ego=cs.aEgo, a_target=lp.aTarget,
        accel_cmd=cc.actuators.accel, long_active=cc.longActive, gas_pressed=cs.gasPressed,
        brake_pressed=cs.brakePressed, long_control_state=int(cc.actuators.longControlState.raw),
        has_lead=lp.hasLead, v_ego_raw=cs.vEgoRaw, blend_ok=blend_ok, brake_pressure=pressure,
      ))

  if len(samples) < 1000:
    print("Not enough samples in this route.")
    return

  d = _resample(samples)
  runs = _engaged_runs(d)
  engaged_min = sum(b - a for a, b in runs) * DT_CTRL / 60
  if car_params is not None:
    print(f"{car_params.carFingerprint}: kpV={list(car_params.longitudinalTuning.kpV)} " +
          f"kiV={list(car_params.longitudinalTuning.kiV)} " +
          f"actuatorDelay={car_params.longitudinalActuatorDelay:.2f}s")
  print(f"{engaged_min:.1f} min of engaged longitudinal in {len(runs)} runs")
  if engaged_min < 2.0:
    print("Too little engaged driving to tune from.")
    return

  summarize_static_map(runs, d)
  summarize_plant(runs, d)
  summarize_tracking(runs, d)

  s = _delay_series(runs, d)
  summarize_measurement_lag(s)
  summarize_delay_split(s)
  summarize_brake_threshold(s)
  summarize_step_events(s)
  summarize_brake_pressure(s)
  if learner is not None:
    learner.report()


if __name__ == "__main__":
  main()
