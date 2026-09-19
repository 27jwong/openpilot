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

Usage:
  ./analyze_longitudinal.py <dongle>/<route>
  ./analyze_longitudinal.py <dongle>/<route> --mode rlog
"""
import argparse
from dataclasses import dataclass

import numpy as np

from openpilot.common.realtime import DT_CTRL
from openpilot.tools.lib.logreader import LogReader, ReadMode

# The car has to hold a near-constant command for this long before the sample counts
# as steady state, otherwise the plant's own lag pollutes the gain estimate.
QSS_WINDOW_S = 1.0
QSS_MAX_COMMAND_SPREAD = 0.15
# Real vehicle acceleration does not live above a couple of Hz; anything faster is
# noise in aEgo, which is a differentiated wheel speed.
NOISE_CUTOFF_HZ = 1.5

LongCtrlStatePid = 1


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


def _resample(samples: list[Sample]) -> dict[str, np.ndarray]:
  """Put every signal on a uniform DT_CTRL grid so the fits below are well posed."""
  t = np.array([s.t for s in samples])
  grid = np.arange(t[0], t[-1], DT_CTRL)
  out = {"t": grid}
  for name in ("v_ego", "a_ego", "a_target", "accel_cmd"):
    out[name] = np.interp(grid, t, np.array([getattr(s, name) for s in samples]))
  for name in ("long_active", "gas_pressed", "brake_pressed", "long_control_state", "has_lead"):
    raw = np.array([float(getattr(s, name)) for s in samples])
    out[name] = raw[np.clip(np.searchsorted(t, grid, side="right") - 1, 0, len(t) - 1)]
  return out


def _engaged_runs(d: dict[str, np.ndarray], min_seconds: float = 3.0):
  """Index pairs where openpilot longitudinal was driving, uninterrupted."""
  m = (d["long_active"] > 0.5) & (d["gas_pressed"] < 0.5) & (d["brake_pressed"] < 0.5)
  m &= np.abs(d["long_control_state"] - LongCtrlStatePid) < 0.1
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


def main() -> None:
  parser = argparse.ArgumentParser(description="Analyze a route for longitudinal tuning opportunities.")
  parser.add_argument("route", help="Route name, e.g. dongle/route")
  parser.add_argument("--mode", choices=("auto", "qlog", "rlog"), default="rlog")
  args = parser.parse_args()

  mode_map = {"auto": ReadMode.AUTO, "qlog": ReadMode.QLOG, "rlog": ReadMode.RLOG}
  log_reader = LogReader(args.route, default_mode=mode_map[args.mode], sort_by_time=True)

  car_params = None
  latest: dict = {}
  samples: list[Sample] = []
  for msg in log_reader:
    which = msg.which()
    if which == "carParams" and car_params is None:
      car_params = msg.carParams
    elif which in ("carControl", "longitudinalPlan"):
      latest[which] = getattr(msg, which)
    elif which == "carState" and "carControl" in latest and "longitudinalPlan" in latest:
      cs, cc, lp = msg.carState, latest["carControl"], latest["longitudinalPlan"]
      samples.append(Sample(
        t=msg.logMonoTime * 1e-9, v_ego=cs.vEgo, a_ego=cs.aEgo, a_target=lp.aTarget,
        accel_cmd=cc.actuators.accel, long_active=cc.longActive, gas_pressed=cs.gasPressed,
        brake_pressed=cs.brakePressed, long_control_state=int(cc.actuators.longControlState.raw),
        has_lead=lp.hasLead,
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


if __name__ == "__main__":
  main()
