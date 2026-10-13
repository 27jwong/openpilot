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

Usage, on the device, by the folder names in /data/media/0/realdata (whole drive, or one segment):
  ./analyze_longitudinal.py 00000005--58d6714619 --replay-learner
  ./analyze_longitudinal.py 00000005--58d6714619--47
  ./analyze_longitudinal.py --all --replay-learner              # pool every drive on the device
  ./analyze_longitudinal.py 00000005--58d6714619 00000006--0a1b2c3d4e   # pool these drives

On a PC, with segment folders copied off the device, or from comma connect:
  ./analyze_longitudinal.py 00000005--58d6714619 --log-dir D:/logs/realdata
  ./analyze_longitudinal.py path/to/00000005--58d6714619--47
  ./analyze_longitudinal.py <dongle>/<route> --mode rlog

Local routes are read from their rlogs, one segment at a time so a long drive fits in memory.
One drive rarely holds enough braking to compare gas and brake; pooled drives usually do. A verdict
is only printed when halves of the data agree on it.
"""
import argparse
import math
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

import numpy as np

from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.locationd import longlagd
from openpilot.selfdrive.locationd.lagd import LateralLagEstimator
from openpilot.starpilot.common.longitudinal_delay import BRAKE_THRESHOLD_BP, brake_threshold
from openpilot.system.hardware.hw import Paths
from openpilot.tools.lib.logreader import LogReader, ReadMode
from openpilot.tools.tuning.train_nnff import SEGMENT_RE, find_segments

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
# Wide enough on the low side that "no separate brake path" shows up as a best fit at the bottom.
THRESHOLD_OFFSETS = np.round(np.arange(-0.8, 0.301, 0.05), 2)
# Less r2 than this across the whole sweep and the data can't place the threshold at all.
FLAT_R2 = 0.01
# The gas-vs-brake verdict is refitted on this many random halves of the engaged driving, cut into
# blocks of up to a minute, and only stated when both delays are pinned down in this share of them.
HALF_SAMPLES = 40
VERDICT_SHARE = 0.8
VERDICT_BLOCK_S = 60.0
MIN_VERDICT_BLOCKS = 6
# Gaps smaller than one planner step don't change what it asks for, so they count as alike.
VERDICT_MIN_GAP_S = 0.05
# Not-engaged padding between pooled drives, so no run, filter or lag window spans two of them.
ROUTE_GAP_S = 2.0


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
  from scipy.signal import lfilter
  alpha = DT_CTRL / (tau + DT_CTRL)
  delayed = np.concatenate((np.full(dead_frames, cmd[0]), cmd[:-dead_frames])) if dead_frames else cmd
  # state += alpha * (x - state), as a filter: hours of pooled driving are too slow in a Python loop
  out, _ = lfilter([alpha], [1.0, alpha - 1.0], delayed, zi=[(1.0 - alpha) * a0])
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


REGIME_BY_NAME = {regime.name: regime for regime in longlagd.REGIMES}


def _pinned(name: str, result) -> bool:
  """Whether the data pins a regime's delay down, by the same tests longlagd applies to a window."""
  regime = REGIME_BY_NAME[name]
  return (result.seconds >= regime.min_okay_sec and result.cmd_range >= regime.min_cmd_range and
          result.sharpness >= longlagd.MIN_SHARPNESS and longlagd.MIN_GAIN <= result.gain <= longlagd.MAX_GAIN and
          longlagd.MIN_LONG_DELAY <= result.delay < longlagd.MAX_LAG)


def _print_fit(label: str, fit) -> None:
  if fit is None:
    print(f"  {label:10s} not enough data")
    return
  parts = [f"{name} {r.delay:.2f}s (gain {r.gain:.2f}, sharp {r.sharpness:.4f}, {r.seconds:4.0f}s)" +
           ("" if _pinned(name, r) else " not pinned down") for name, r in fit.regimes.items()]
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
  print("  The plant deadtime above is fitted to aEgo, so about this much of it is aEgo's own lag.")


def summarize_delay_split(s) -> None:
  """Fit longlagd's two-path model: the powertrain answers down to the brake threshold after one
  delay, the brakes the rest after another. Delays here include the plant's lag, so they are the
  lookaheads the planner wants rather than pure deadtime."""
  print("\nDelay by regime (longlagd's two-path fit, at the current brake threshold table):")
  _print_fit("all", longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"], s["usable"], DELAY_DT))
  for label, _, _, band in _bands(s):
    _print_fit(label, longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"], band, DELAY_DT))


def _classify_sweep(r2s: list[float | None]) -> tuple[str, int | None]:
  """Read a threshold sweep: 'flat' (can't place it), 'single' (best with no brake path in range),
  'edge' (best at the top), or 'interior' with the index of the best offset."""
  values = [r for r in r2s if r is not None]
  if len(values) < 3:
    return "no data", None
  if max(values) - min(values) < FLAT_R2:
    return "flat", None
  best = max(range(len(r2s)), key=lambda i: -math.inf if r2s[i] is None else r2s[i])
  if best == 0:
    return "single", best
  if best == len(r2s) - 1:
    return "edge", best
  return "interior", best


SWEEP_VERDICTS = {
  "no data": "not enough data",
  "flat": "flat: this data can't place the threshold",
  "single": "a single delay fits best (no separate brake path in range)",
  "edge": "best at the top of the sweep: can't place it",
}


def summarize_brake_threshold(s) -> tuple[str, float | None]:
  """Move the assumed brake threshold and see where the two-path model fits best. The threshold
  is where requests stop being met by closing the throttle, so it should get more negative
  with speed as drag and engine braking grow. Returns how the all-speeds sweep reads, and its
  best offset when there is an interior one."""
  print("\nBrake threshold sweep (r2 of the two-path fit, offset from the current table):")
  columns = [("all", 0.0, math.inf, s["usable"]), *_bands(s)]
  print(f"  {'offset':>7} " + " ".join(f"{label:>10}" for label, *_ in columns))
  r2s: dict[str, list[float | None]] = {label: [] for label, *_ in columns}
  fits: dict[str, list] = {label: [] for label, *_ in columns}
  for offset in THRESHOLD_OFFSETS:
    row = []
    for label, _, _, mask in columns:
      fit = longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"] + offset, mask, DELAY_DT)
      r2s[label].append(fit.r2 if fit is not None else None)
      fits[label].append(fit)
      row.append(f"{fit.r2:10.3f}" if fit is not None else f"{'-':>10}")
    print(f"  {offset:+7.2f} " + " ".join(row))

  centers, values = [], []
  for label, lo, hi, _ in columns:
    kind, best = _classify_sweep(r2s[label])
    if kind == "interior":
      brake = fits[label][best].regimes["brake"]
      if not _pinned("brake", brake):
        print(f"  {label:10s} best at {THRESHOLD_OFFSETS[best]:+.2f}, but its braking isn't pinned down ({brake.seconds:.0f}s)")
        continue
      print(f"  {label:10s} best at {THRESHOLD_OFFSETS[best]:+.2f}")
      if label != "all":
        center = (lo + min(hi, 35.0)) / 2
        centers.append(center)
        values.append(round(brake_threshold(center) + float(THRESHOLD_OFFSETS[best]), 2))
    else:
      print(f"  {label:10s} {SWEEP_VERDICTS[kind]}")
  if centers:
    print(f"  current: BRAKE_THRESHOLD_BP = {BRAKE_THRESHOLD_BP}")
    print(f"  suggest: BRAKE_THRESHOLD_BP = {centers}, BRAKE_THRESHOLD_V = {values}")

  kind, best = _classify_sweep(r2s["all"])
  return kind, (float(THRESHOLD_OFFSETS[best]) if kind == "interior" else None)


def _blocks_of(mask: np.ndarray, block: int) -> list[tuple[int, int]]:
  """Contiguous stretches of mask, cut into pieces of at most block samples."""
  edges = np.flatnonzero(np.diff(np.concatenate(([0], mask.astype(np.int8), [0]))))
  return [(a, min(a + block, b)) for start, b in zip(edges[::2], edges[1::2], strict=True) for a in range(start, b, block)]


def gas_brake_verdict(s, sweep: str, offset: float | None, seed: int = 0) -> str:
  """Compare the brake and gas delays only as far as the data supports: refit on random halves of
  the engaged driving (in blocks of up to a minute), and require both delays to be pinned down in
  most halves before saying anything. A gap whose spread crosses zero means they answer alike."""
  blocks = _blocks_of(s["usable"], int(round(VERDICT_BLOCK_S / DELAY_DT)))
  if len(blocks) < MIN_VERDICT_BLOCKS:
    return f"Only {len(blocks)} minute-long blocks of engaged driving: too few to check a gas/brake gap holds up."
  rng = np.random.default_rng(seed)
  thr = s["thr"] + (offset if offset is not None else 0.0)
  gaps, pinned = [], 0
  for _ in range(HALF_SAMPLES):
    mask = np.zeros_like(s["usable"])
    for i in rng.choice(len(blocks), size=len(blocks) // 2, replace=False):
      mask[blocks[i][0]:blocks[i][1]] = True
    fit = longlagd.fit_split_delay(s["cmd"], s["accel"], thr, mask & s["usable"], DELAY_DT)
    if fit is not None and all(_pinned(name, r) for name, r in fit.regimes.items()):
      pinned += 1
      gaps.append(fit.regimes["brake"].delay - fit.regimes["gas"].delay)

  if pinned < VERDICT_SHARE * HALF_SAMPLES:
    full = longlagd.fit_split_delay(s["cmd"], s["accel"], thr, s["usable"], DELAY_DT)
    brake_s = full.regimes["brake"].seconds if full is not None else 0.0
    return (f"Can't tell: both delays were pinned down in only {pinned} of {HALF_SAMPLES} halves of the data " +
            f"({brake_s:.0f}s in the brake range).")
  lo, mid, hi = (1000 * np.percentile(gaps, q) for q in (10, 50, 90))
  spread = f"brake minus gas {mid:+.0f} ms, {lo:+.0f} to {hi:+.0f} ms across halves of the data"
  margin = 1000 * VERDICT_MIN_GAP_S
  if -margin <= lo and hi <= margin:
    return f"Gas and brake answer alike, within {margin:.0f} ms: {spread}."
  if lo <= margin and hi >= -margin:
    return f"Can't tell: {spread}."
  if sweep != "interior":
    return f"Tentative, since the data can't place the brake threshold: {spread}."
  return (f"The brakes answer {abs(mid):.0f} ms {'later' if mid > 0 else 'sooner'} than the powertrain " +
          f"({spread}; threshold offset {offset:+.2f}).")


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
  # the planner ramps more than it steps, so a change within a second counts
  pre, rise, settle = int(round(0.5 / dt)), int(round(1.0 / dt)), int(round(1.5 / dt))

  events: dict[tuple[str, str], list[float]] = {}
  i = pre
  while i + rise + settle < len(cmd):
    span = slice(i - pre, i + rise + settle)
    c0 = cmd[i - pre:i]
    step = cmd[i + rise] - c0.mean()
    if not usable[span].all() or np.ptp(c0) > 0.15 or abs(step) < 0.3:
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
  its state, livePose clocks the points, and every fifth livePose an estimate. A drive can start
  from where the previous one left off, as it does on the car."""
  TICK_S = 0.25  # one estimate per five 20Hz livePose frames

  def __init__(self, CP, initial: dict | None = None, **estimator_kwargs):
    self.estimator = longlagd.LongitudinalLagEstimator(CP, 0.05, **estimator_kwargs)
    if initial:
      self.estimator.reset(initial)
    self.frame = 0
    self.car_state_updated = False
    self.t0: float | None = None
    self.history: list[tuple] = []
    self.outcomes = {regime.name: Counter() for regime in longlagd.REGIMES}
    self.r2: list[float] = []

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
        for name, outcome in self.estimator.last_outcome.items():
          self.outcomes[name][outcome] += 1
        if self.estimator.last_fit is not None:
          self.r2.append(self.estimator.last_fit.r2)
        ld = self.estimator.get_msg(True).starpilotLongitudinalDelay
        self.history.append((t - self.t0, ld.gas.status, ld.gas.delayEstimate, ld.gas.validBlocks,
                             ld.brake.status, ld.brake.delayEstimate, ld.brake.validBlocks))
      self.frame += 1

  def final_state(self) -> dict[str, tuple[float, int]]:
    return longlagd.initial_from_saved(self.estimator.get_msg(True).starpilotLongitudinalDelay)


def report_replay(replays: list[tuple[str, LearnerReplay]]) -> None:
  replays = [(name, r) for name, r in replays if r.history]
  print("\nlonglagd replay" + (f" across {len(replays)} drives, carrying over between them:" if len(replays) > 1 else ":"))
  if not replays:
    print("  no livePose in these logs, so nothing clocked the learner")
    return

  elapsed = 0.0
  for offset, name in ((1, "gas"), (4, "brake")):
    first, elapsed = None, 0.0
    for _, replay in replays:
      hit = next((h[0] for h in replay.history if h[offset] == "estimated"), None)
      if hit is not None and first is None:
        first = elapsed + hit
      elapsed += replay.history[-1][0]
    last = replays[-1][1].history[-1]
    when = f"estimated after {first / 60:.1f} min of logs" if first is not None else "never estimated"
    print(f"  {name:5s} {when}, ends at {last[offset + 1]:.3f}s with {last[offset + 2]} blocks ({last[offset]})")

  if len(replays) == 1:
    history = replays[0][1].history
    print(f"  {'min':>6} {'gas':>16} {'brake':>16}")
    for h in history[::max(1, len(history) // 12)]:
      print(f"  {h[0] / 60:6.1f} {h[2]:7.3f}s {h[3]:2d} blk {h[5]:7.3f}s {h[6]:2d} blk")
  else:
    print(f"  {'drive':24s} {'min':>5} {'gas':>16} {'brake':>16}")
    for name, replay in replays:
      h = replay.history[-1]
      print(f"  {name:24s} {h[0] / 60:5.1f} {h[2]:7.3f}s {h[3]:2d} blk {h[5]:7.3f}s {h[6]:2d} blk")

  # what kept the learner from taking estimates, to tune its thresholds against
  print("  why windows were or weren't learned from (minutes; 'no fit' includes time not engaged):")
  for regime in longlagd.REGIMES:
    total = sum((r.outcomes[regime.name] for _, r in replays), Counter())
    parts = [f"{outcome} {count * LearnerReplay.TICK_S / 60:.1f}" for outcome, count in total.most_common()]
    print(f"    {regime.name:5s} " + ", ".join(parts))
  r2 = np.concatenate([r.r2 for _, r in replays]) if any(r.r2 for _, r in replays) else np.array([])
  if len(r2):
    p10, p50, p90 = np.percentile(r2, [10, 50, 90])
    print(f"  window r2: 10% {p10:.2f}, median {p50:.2f}, 90% {p90:.2f}; " +
          f"min r2 {replays[-1][1].estimator.min_r2} passes {100 * np.mean(r2 >= replays[-1][1].estimator.min_r2):.0f}%")


def resolve_route(identifier: str, log_dirs: list[Path]) -> list[str]:
  """What to hand LogReader, one entry per read. A local route or segment, named the way its
  folders are in /data/media/0/realdata, becomes its rlogs in segment order, so each segment can
  be read on its own. Anything else (dongle/route, a file, a URL) is passed through as it is."""
  path = Path(identifier)
  if path.is_dir():
    routes = find_segments([path])
    if len(routes) != 1:
      raise SystemExit(f"{identifier} holds {len(routes)} routes' segments; point at one route's folders")
    return [str(p) for _, p in next(iter(routes.values()))]
  if path.exists() or "/" in identifier or "|" in identifier:
    return [identifier]

  searched = [d for d in log_dirs if d.is_dir()]
  routes = find_segments(searched)
  # a whole route first: its random part can be all digits and look like a segment number
  if identifier in routes:
    return [str(p) for _, p in routes[identifier]]
  m = SEGMENT_RE.match(identifier)
  if m is not None and m["route"] in routes:
    segment = [str(p) for n, p in routes[m["route"]] if n == int(m["segment"])]
    if segment:
      print(f"Reading one segment (about a minute). Give {m['route']} for the whole drive.")
      return segment
  where = ', '.join(map(str, log_dirs)) or 'no log folders'
  raise SystemExit(f"No rlogs for {identifier!r} in {where}. Give a route or segment folder name, a path, or dongle/route.")


def read_route(identifiers: list[str], mode: ReadMode, replay: bool, initial: dict | None, learner_kwargs: dict):
  """One drive on the DT_CTRL grid (None if it's too short), its CarParams, and longlagd replayed
  over it when asked. Only the resampled arrays are kept, so pooled drives stay small."""
  # A fresh LogReader per local segment: one LogReader keeps every segment it has read in memory.
  messages = (msg for identifier in identifiers for msg in LogReader(identifier, default_mode=mode, sort_by_time=True))

  car_params = None
  latest: dict = {}
  samples: list[Sample] = []
  learner: LearnerReplay | None = None
  pressure_probe = None
  pressure = math.nan
  blend_ok = True
  for msg in messages:
    which = msg.which()
    if learner is not None:
      learner.feed(msg)
    if which == "carParams" and car_params is None:
      car_params = msg.carParams.as_builder()  # a copy, so the first segment isn't held for it
      pressure_probe = _brake_pressure_probe(car_params)
      if replay:
        learner = LearnerReplay(car_params, initial, **learner_kwargs)
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
  return (_resample(samples) if len(samples) >= 1000 else None), car_params, learner


def _concat_routes(ds: list[dict[str, np.ndarray]]) -> dict[str, np.ndarray]:
  """Pool drives into one set of arrays: time runs on across them, and a stretch of not-engaged
  padding in between keeps every engaged run, filter and lag window inside a single drive."""
  if len(ds) == 1:
    return ds[0]
  gap = int(round(ROUTE_GAP_S / DT_CTRL))
  parts: dict[str, list[np.ndarray]] = {key: [] for key in ds[0]}
  t_next = 0.0
  for i, d in enumerate(ds):
    if i:
      for key in parts:
        if key == "t":
          parts[key].append(t_next + np.arange(gap) * DT_CTRL)
        elif key in ("v_ego", "v_ego_raw"):
          parts[key].append(np.full(gap, d[key][0]))
        else:
          parts[key].append(np.full(gap, math.nan if key == "brake_pressure" else 0.0))
      t_next += gap * DT_CTRL
    for key in parts:
      parts[key].append(d[key] + (t_next - d["t"][0]) if key == "t" else d[key])
    t_next = parts["t"][-1][-1] + DT_CTRL
  return {key: np.concatenate(value) for key, value in parts.items()}


def coverage(d: dict[str, np.ndarray]) -> Counter:
  """Seconds of a drive by what happened to them, most of which never reach the fits."""
  active = d["long_active"] > 0.5
  blended = active & (d["blend_ok"] < 0.5)
  pedals = active & ~blended & ((d["gas_pressed"] > 0.5) | (d["brake_pressed"] > 0.5))
  not_pid = active & ~blended & ~pedals & (np.abs(d["long_control_state"] - LongCtrlStatePid) > 0.1)
  ok = active & ~blended & ~pedals & ~not_pid
  usable = sum(b - a for a, b in _engaged_runs(d))
  seconds = {"logs": len(d["t"]), "active": active.sum(), "blended": blended.sum(), "pedals": pedals.sum(),
             "not_pid": not_pid.sum(), "short": ok.sum() - usable, "usable": usable}
  return Counter({key: float(value) * DT_CTRL for key, value in seconds.items()})


def summarize_coverage(total: Counter, drives: int) -> None:
  m = {key: total.get(key, 0.0) / 60 for key in ("logs", "active", "usable", "blended", "pedals", "not_pid", "short")}
  print(f"{m['logs']:.1f} min of logs" + (f" over {drives} drives" if drives > 1 else "") +
        f"; openpilot long active {m['active']:.1f}; usable {m['usable']:.1f} " +
        f"(blended to stock ACC {m['blended']:.1f}, pedals {m['pedals']:.1f}, stopping/starting {m['not_pid']:.1f}, " +
        f"runs under 3 s {m['short']:.1f})")


def main() -> None:
  parser = argparse.ArgumentParser(description="Analyze drives for longitudinal tuning opportunities.")
  parser.add_argument("routes", nargs="*",
                      help="local routes or segments (00000005--58d6714619[--47]), paths, or dongle/route; several are pooled")
  parser.add_argument("--all", action="store_true", help="pool every local route in the log folders")
  parser.add_argument("--log-dir", action="append", type=Path, default=[],
                      help="also look for local routes here (default: the device's realdata folder)")
  parser.add_argument("--mode", choices=("auto", "qlog", "rlog"), default="rlog", help="for dongle/route; local routes use rlogs")
  parser.add_argument("--replay-learner", action="store_true", help="run longlagd over the drives and report how it converges")
  parser.add_argument("--learner-min-r2", type=float, default=longlagd.MIN_R2, help="replay with this window r2 threshold")
  parser.add_argument("--learner-min-sharpness", type=float, default=longlagd.MIN_SHARPNESS,
                      help="replay with this delay sharpness threshold")
  args = parser.parse_args()

  log_dirs = [Path(Paths.log_root()), *args.log_dir]
  if args.all:
    routes = list(find_segments([d for d in log_dirs if d.is_dir()]))
  else:
    routes = args.routes
  if not routes:
    parser.error("give one or more routes, or --all")

  mode_map = {"auto": ReadMode.AUTO, "qlog": ReadMode.QLOG, "rlog": ReadMode.RLOG}
  learner_kwargs = {"min_r2": args.learner_min_r2, "min_sharpness": args.learner_min_sharpness}
  car_params = None
  ds: list[dict[str, np.ndarray]] = []
  total = Counter()
  replays: list[tuple[str, LearnerReplay]] = []
  initial = None
  for name in routes:
    d, cp, learner = read_route(resolve_route(name, log_dirs), mode_map[args.mode], args.replay_learner, initial, learner_kwargs)
    if cp is not None and car_params is not None and cp.carFingerprint != car_params.carFingerprint:
      print(f"{name}: skipped, it's a {cp.carFingerprint}, not a {car_params.carFingerprint}")
      continue
    if car_params is None:
      car_params = cp
    if learner is not None:
      replays.append((name, learner))
      initial = learner.final_state()
    if d is None:
      print(f"{name}: too short, skipped")
      continue
    drive = coverage(d)
    total.update(drive)  # not +=, which drops the zero counts
    ds.append(d)
    if len(routes) > 1:
      print(f"{name}: {drive['logs'] / 60:.1f} min, {drive['usable'] / 60:.1f} usable")

  if not ds:
    print("Not enough samples in these logs.")
    return

  d = _concat_routes(ds)
  runs = _engaged_runs(d)
  engaged_min = sum(b - a for a, b in runs) * DT_CTRL / 60
  if car_params is not None:
    print(f"{car_params.carFingerprint}: kpV={list(car_params.longitudinalTuning.kpV)} " +
          f"kiV={list(car_params.longitudinalTuning.kiV)} " +
          f"actuatorDelay={car_params.longitudinalActuatorDelay:.2f}s")
  summarize_coverage(total, len(ds))
  print(f"{engaged_min:.1f} min of engaged longitudinal in {len(runs)} runs")
  if engaged_min < 2.0:
    print("Too little engaged driving to tune from.")
    if replays:
      report_replay(replays)
    return

  summarize_static_map(runs, d)
  summarize_plant(runs, d)
  summarize_tracking(runs, d)

  s = _delay_series(runs, d)
  summarize_measurement_lag(s)
  summarize_delay_split(s)
  sweep, offset = summarize_brake_threshold(s)
  print("\nGas vs brake: " + gas_brake_verdict(s, sweep, offset))
  summarize_step_events(s)
  summarize_brake_pressure(s)
  if replays:
    report_replay(replays)


if __name__ == "__main__":
  main()
