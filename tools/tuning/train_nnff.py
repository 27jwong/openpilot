#!/usr/bin/env python3
"""Train an NNFF (neural network lateral feedforward) model for your car from your own rlogs.

The result is a JSON model in the format LatControlNNFF loads from starpilot/assets/nnff_models/.
This script only needs numpy, pycapnp and zstandard plus this repo checked out (for the cereal
schema), not a built openpilot, so it runs on Windows:

  pip install numpy pycapnp zstandard
  python tools/tuning/train_nnff.py D:/logs/realdata
  python tools/tuning/train_nnff.py D:/logs/realdata --siglin 6.69417 0.71168 0.21504 0.02331

Getting logs: copy segment folders out of /data/media/0/realdata/ on the device (WinSCP is fine).
Each folder is one minute of driving named <route>--<segment>, holding an rlog (rlog.zst on this
fork; plain rlog and rlog.bz2 also work). Copy whole routes where you can. qlogs are too sparse.
Only time openpilot was steering is used, and more varied driving (curves, ramps, both
directions, a range of speeds) matters more than more hours of straight highway.

What it learns: the torque openpilot actually sent (carOutput) from the lateral acceleration that
torque produced, which arrives one steering lag later. Rows are built the way LatControlNNFF builds
its inputs at runtime: torque at t against lateral accel, jerk and roll at t + lag, plus lateral
accel and roll 0.1-0.3 s before and 0.3-1.5 s after that. The lag is whatever lagd was publishing
(liveDelay) at that moment, which follows speed, the same value LatControlNNFF shifts its inputs
by on the road.

Routes, not rows, are held out for validation: neighboring 10 ms rows are near copies, so a
random row split mostly measures memorization. The model only earns its place if it beats the
--siglin baseline on routes it never saw.

Install the result by copying it to starpilot/assets/nnff_models/<fingerprint>.json (in your
fork, or straight onto the device under /data/openpilot), then turn on NNFF in the lateral
settings and reboot.
"""
import argparse
import bz2
import datetime
import hashlib
import json
import os
import re
import sys
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor
from dataclasses import dataclass
from pathlib import Path

import numpy as np

REPO_ROOT = Path(__file__).resolve().parents[2]

DT = 0.01
GRAVITY = 9.81
EXTRACT_VERSION = 1  # bump when extract_segment's output changes, to invalidate caches

# These must match LatControlNNFF (starpilot/controls/lib/neural_network_feedforward.py):
# past_times, future_times, and the order it assembles nn_input in.
PAST_TIMES = [-0.3, -0.2, -0.1]
FUTURE_TIMES = [0.3, 0.6, 1.0, 1.5]
HISTORY_FRAMES = [int(round(t / DT)) for t in PAST_TIMES + FUTURE_TIMES]
INPUT_VARS = ['v_ego', 'lateral_accel', 'lateral_jerk', 'roll',
              'lateral_accel_m03', 'lateral_accel_m02', 'lateral_accel_m01',
              'lateral_accel_p03', 'lateral_accel_p06', 'lateral_accel_p10', 'lateral_accel_p15',
              'roll_m03', 'roll_m02', 'roll_m01', 'roll_p03', 'roll_p06', 'roll_p10', 'roll_p15']

# Fallback when a route has no liveDelay: CP.steerActuatorDelay plus the software delay lagd
# starts from (LATERAL_CONTROL_SOFTWARE_DELAY in starpilot/common/lateral_delay.py)
LATERAL_CONTROL_SOFTWARE_DELAY = 0.2

ENGAGED_BEFORE = 0.5      # s of uninterrupted steering required before a row's torque sample
SATURATED_TORQUE = 0.98   # at the limit the car got less torque than it needed, so skip those
JERK_SMOOTH = 0.1         # s, moving average before differentiating lateral accel
MAX_AGE_FAST = 0.05       # s, staleness limit for the 100 Hz services
MAX_AGE_SLOW = 0.3        # s, staleness limit for liveParameters (20 Hz)

SEGMENT_RE = re.compile(r"^(?P<route>.+)--(?P<segment>\d+)$")
RLOG_NAMES = ("rlog.zst", "rlog.bz2", "rlog")


# *** reading logs ***

_log_schema = None


def load_log_schema(cereal_dir: Path | None = None):
  global _log_schema
  if _log_schema is None:
    if cereal_dir is None:
      sys.path.insert(0, str(REPO_ROOT))
      from cereal import log
      _log_schema = log
    else:
      import capnp
      capnp.remove_import_hook()
      _log_schema = capnp.load(str(Path(cereal_dir) / "log.capnp"), imports=[str(cereal_dir)])
  return _log_schema


def read_log_bytes(path: Path) -> bytes:
  data = path.read_bytes()
  if path.suffix == ".zst" or data[:4] == b"\x28\xb5\x2f\xfd":
    import zstandard
    reader = zstandard.ZstdDecompressor().stream_reader(data, read_across_frames=True)
    chunks = []
    try:
      while chunk := reader.read(1 << 20):
        chunks.append(chunk)
    except zstandard.ZstdError:
      pass  # truncated by a power cut: keep what decompressed
    return b"".join(chunks)
  if path.suffix == ".bz2" or data[:3] == b"BZh":
    return bz2.decompress(data)
  return data


def extract_segment(path: Path, cereal_dir: Path | None = None) -> dict[str, np.ndarray]:
  """Pull the handful of signals training needs out of one rlog, at their native rates."""
  log = load_log_schema(cereal_dir)
  cols = defaultdict(list)
  meta = {"fingerprint": "", "steer_actuator_delay": np.nan}
  try:
    for evt in log.Event.read_multiple_bytes(read_log_bytes(path)):
      which = evt.which()
      t = evt.logMonoTime * 1e-9
      if which == "carState":
        cs = evt.carState
        cols["cs_t"].append(t)
        cols["v_ego"].append(cs.vEgo)
        cols["steering_pressed"].append(cs.steeringPressed)
        cols["steer_fault"].append(cs.steerFaultTemporary or cs.steerFaultPermanent)
      elif which == "carControl":
        cols["cc_t"].append(t)
        cols["lat_active"].append(evt.carControl.latActive)
      elif which == "carOutput":
        cols["co_t"].append(t)
        cols["torque"].append(evt.carOutput.actuatorsOutput.torque)
      elif which == "controlsState":
        cols["ctl_t"].append(t)
        cols["curvature"].append(evt.controlsState.curvature)
      elif which == "liveParameters":
        cols["lp_t"].append(t)
        cols["roll"].append(evt.liveParameters.roll)
      elif which == "liveDelay":
        cols["ld_t"].append(t)
        cols["lateral_delay"].append(evt.liveDelay.lateralDelay)
      elif which == "carParams" and not meta["fingerprint"]:
        meta["fingerprint"] = evt.carParams.carFingerprint
        meta["steer_actuator_delay"] = evt.carParams.steerActuatorDelay
  except Exception as e:  # a truncated final message; everything before it is usable
    print(f"warning: {path}: stopped reading at {e!r}", file=sys.stderr)

  out = {k: np.asarray(v, dtype=np.float64) for k, v in cols.items()}
  out["fingerprint"] = np.asarray(meta["fingerprint"])
  out["steer_actuator_delay"] = np.asarray(meta["steer_actuator_delay"])
  return out


def _cache_path(cache_dir: Path, path: Path) -> Path:
  st = path.stat()
  key = f"{EXTRACT_VERSION}|{path.resolve()}|{st.st_size}|{st.st_mtime_ns}"
  return cache_dir / f"{path.parent.name}-{hashlib.sha1(key.encode()).hexdigest()[:12]}.npz"


def extract_segment_cached(path: Path, cache_dir: Path | None, cereal_dir: Path | None) -> dict[str, np.ndarray]:
  if cache_dir is None:
    return extract_segment(path, cereal_dir)
  cached = _cache_path(cache_dir, path)
  if cached.exists():
    with np.load(cached) as f:
      return dict(f)
  out = extract_segment(path, cereal_dir)
  cache_dir.mkdir(parents=True, exist_ok=True)
  tmp = cached.with_suffix(".tmp.npz")
  np.savez(tmp, **out)
  os.replace(tmp, cached)
  return out


def find_segments(log_dirs: list[Path]) -> dict[str, list[tuple[int, Path]]]:
  """route name -> [(segment number, rlog path)], from folders named <route>--<segment>."""
  routes: dict[str, dict[int, Path]] = defaultdict(dict)
  for log_dir in log_dirs:
    for name in RLOG_NAMES:
      for path in Path(log_dir).rglob(name):
        m = SEGMENT_RE.match(path.parent.name)
        if m is None:
          print(f"warning: skipping {path}: its folder isn't named <route>--<segment>", file=sys.stderr)
          continue
        routes[m["route"]].setdefault(int(m["segment"]), path)
  return {route: sorted(segs.items()) for route, segs in sorted(routes.items())}


# *** per-route signals on a uniform grid ***

@dataclass
class Route:
  name: str
  fingerprint: str
  lag: np.ndarray         # s, the liveDelay in effect at each grid point (NaN if the route logged none)
  v_ego: np.ndarray
  lat_accel: np.ndarray   # controlsState.curvature * vEgo^2: the same measurement LatControlNNFF uses
  lat_jerk: np.ndarray
  roll: np.ndarray
  torque: np.ndarray      # in LatControlNNFF's output convention (it sends -output_torque)
  ok: np.ndarray          # openpilot steering, no driver override or fault, every signal fresh


def _moving_average(x: np.ndarray, n: int) -> np.ndarray:
  if n <= 1:
    return x
  pad = np.pad(x, (n // 2, n - 1 - n // 2), mode="edge")
  return np.convolve(pad, np.ones(n) / n, mode="valid")


def assemble_route(name: str, segments: list[dict[str, np.ndarray]]) -> Route | None:
  cat = {}
  for key in ("cs_t", "v_ego", "steering_pressed", "steer_fault", "cc_t", "lat_active", "co_t", "torque",
              "ctl_t", "curvature", "lp_t", "roll", "ld_t", "lateral_delay"):
    cat[key] = np.concatenate([np.zeros(0)] + [s[key] for s in segments if key in s])
  for t_key, service in (("cs_t", "carState"), ("cc_t", "carControl"), ("co_t", "carOutput"), ("ctl_t", "controlsState"),
                         ("lp_t", "liveParameters")):
    if len(cat[t_key]) < 2:
      print(f"warning: route {name} has no {service} messages (a qlog, or a log from before it existed), skipping", file=sys.stderr)
      return None

  fingerprints = Counter(str(s["fingerprint"]) for s in segments if str(s["fingerprint"]))
  fingerprint = fingerprints.most_common(1)[0][0] if fingerprints else ""
  delays = [float(s["steer_actuator_delay"]) for s in segments if np.isfinite(s["steer_actuator_delay"])]

  grid = np.arange(cat["cs_t"][0], cat["cs_t"][-1], DT)

  def latest(t_key, max_age):
    t = cat[t_key]
    order = np.argsort(t, kind="stable")
    idx = np.searchsorted(t[order], grid, side="right") - 1
    fresh = (idx >= 0) & (grid - t[order][np.maximum(idx, 0)] <= max_age)
    return order[np.maximum(idx, 0)], fresh

  if len(cat["lateral_delay"]):
    # lagd interpolates its delay by speed and publishes at 4 Hz, so hold the latest value; the
    # stretch before a route's first liveDelay borrows that first value
    ld_idx, _ = latest("ld_t", np.inf)
    lag = cat["lateral_delay"][ld_idx]
  elif delays:
    lag = np.full(len(grid), delays[0] + LATERAL_CONTROL_SOFTWARE_DELAY)
  else:
    lag = np.full(len(grid), np.nan)  # usable only with --lag

  def interp(t_key, key):
    order = np.argsort(cat[t_key], kind="stable")
    return np.interp(grid, cat[t_key][order], cat[key][order])

  cs_idx, cs_fresh = latest("cs_t", MAX_AGE_FAST)
  cc_idx, cc_fresh = latest("cc_t", MAX_AGE_FAST)
  _, co_fresh = latest("co_t", MAX_AGE_FAST)
  _, ctl_fresh = latest("ctl_t", MAX_AGE_FAST)
  _, lp_fresh = latest("lp_t", MAX_AGE_SLOW)

  v_ego = interp("cs_t", "v_ego")
  lat_accel = interp("ctl_t", "curvature") * v_ego ** 2
  smooth = _moving_average(lat_accel, int(round(JERK_SMOOTH / DT)) + 1)
  ok = (cs_fresh & cc_fresh & co_fresh & ctl_fresh & lp_fresh &
        (cat["lat_active"][cc_idx] > 0.5) & (cat["steering_pressed"][cs_idx] < 0.5) & (cat["steer_fault"][cs_idx] < 0.5))
  return Route(
    name=name,
    fingerprint=fingerprint,
    lag=lag,
    v_ego=v_ego,
    lat_accel=lat_accel,
    lat_jerk=np.gradient(smooth, DT),
    roll=interp("lp_t", "roll"),
    torque=-interp("co_t", "torque"),
    ok=ok,
  )


# *** training rows ***

def lag_frames(route: Route, lag: float | None = None) -> np.ndarray:
  """Per grid point, how many frames after a torque sample its lateral accel shows up."""
  lag_s = route.lag if lag is None else np.full(len(route.v_ego), lag)
  return np.rint(np.nan_to_num(lag_s, nan=0.0) / DT).astype(int)


def row_indices(route: Route, lag: float | None = None, stride: int = 5, min_speed: float = 3.0) -> np.ndarray:
  """Grid indices whose whole window, from ENGAGED_BEFORE ahead of the torque sample to the last
  future input, is openpilot steering undisturbed."""
  pre = int(round(ENGAGED_BEFORE / DT))
  n = len(route.v_ego)
  i = np.arange(pre, n)
  i = i[i % stride == 0]
  end = i + lag_frames(route, lag)[i] + max(HISTORY_FRAMES)
  i, end = i[end < n], end[end < n]
  bad = np.concatenate(([0], np.cumsum(~route.ok)))
  usable = ((bad[end + 1] - bad[i - pre]) == 0) & (route.v_ego[i] >= min_speed) & (np.abs(route.torque[i]) < SATURATED_TORQUE)
  return i[usable]


def build_rows(route: Route, lag: float | None = None, stride: int = 5, min_speed: float = 3.0) -> tuple[np.ndarray, np.ndarray]:
  """X in INPUT_VARS order and y (torque) for every usable grid index, decimated by stride."""
  i = row_indices(route, lag, stride, min_speed)
  c = i + lag_frames(route, lag)[i]  # where the "current" inputs sit: one lag after the torque
  la, roll = route.lat_accel, route.roll
  X = np.column_stack([route.v_ego[i], la[c], route.lat_jerk[c], roll[c]] +
                      [la[c + k] for k in HISTORY_FRAMES] + [roll[c + k] for k in HISTORY_FRAMES])
  return X.reshape(-1, len(INPUT_VARS)), route.torque[i]


def mirror(X: np.ndarray, y: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
  """Add the left/right mirror image of every row: everything but speed flips sign."""
  Xm = -X
  Xm[:, 0] = X[:, 0]
  return np.concatenate([X, Xm]), np.concatenate([y, -y])


def balance_weights(lateral_accel: np.ndarray, bin_width: float = 0.25, max_weight: float = 10.0) -> np.ndarray:
  """Upweight rarer, larger lateral accels so straight driving doesn't drown out the curves."""
  bins = np.minimum(np.abs(lateral_accel) // bin_width, 16).astype(int)
  counts = np.bincount(bins)
  return np.minimum(np.sqrt(counts.max() / counts[bins]), max_weight)


# *** model ***

def sigmoid(x: np.ndarray) -> np.ndarray:
  return 0.5 * (1.0 + np.tanh(0.5 * x))


class MLP:
  """Sigmoid hidden layers and a linear output: the subset of FluxModel's format this exports to."""

  def __init__(self, sizes: list[int], rng: np.random.Generator):
    self.W = [rng.uniform(-1, 1, (a, b)) * np.sqrt(6.0 / (a + b)) for a, b in zip(sizes[:-1], sizes[1:], strict=True)]
    self.b = [np.zeros(b) for b in sizes[1:]]

  def forward(self, X: np.ndarray) -> list[np.ndarray]:
    acts = [X]
    for k, (W, b) in enumerate(zip(self.W, self.b, strict=True)):
      z = acts[-1] @ W + b
      acts.append(z if k == len(self.W) - 1 else sigmoid(z))
    return acts

  def predict(self, X: np.ndarray) -> np.ndarray:
    return self.forward(X)[-1][:, 0]

  def loss_and_grads(self, X: np.ndarray, y: np.ndarray, w: np.ndarray, huber_delta: float):
    acts = self.forward(X)
    r = acts[-1][:, 0] - y
    quad = np.abs(r) <= huber_delta
    loss = np.where(quad, 0.5 * r ** 2, huber_delta * (np.abs(r) - 0.5 * huber_delta))
    g = (w * np.where(quad, r, huber_delta * np.sign(r)) / w.sum())[:, None]
    gW, gb = [None] * len(self.W), [None] * len(self.W)
    for k in reversed(range(len(self.W))):
      gW[k] = acts[k].T @ g
      gb[k] = g.sum(axis=0)
      if k > 0:
        g = (g @ self.W[k].T) * acts[k] * (1.0 - acts[k])
    return float((w * loss).sum() / w.sum()), gW, gb

  def copy(self) -> "MLP":
    other = MLP.__new__(MLP)
    other.W = [W.copy() for W in self.W]
    other.b = [b.copy() for b in self.b]
    return other


@dataclass
class TrainResult:
  model: MLP
  mean: np.ndarray
  std: np.ndarray
  val_loss: float
  epochs: int


def train(X_train, y_train, w_train, X_val, y_val, w_val, hidden=(24, 12), epochs=60, batch_size=2048, lr=3e-3,
          weight_decay=1e-5, huber_delta=0.1, patience=8, seed=0, verbose=True) -> TrainResult:
  rng = np.random.default_rng(seed)
  mean = X_train.mean(axis=0)
  std = np.maximum(X_train.std(axis=0), 1e-3)
  Xt = (X_train - mean) / std
  Xv = (X_val - mean) / std
  # keep at least ~50 steps an epoch when there isn't much data
  batch_size = min(batch_size, max(64, len(Xt) // 50))

  model = MLP([X_train.shape[1], *hidden, 1], rng)
  params = model.W + model.b
  m = [np.zeros_like(p) for p in params]
  v = [np.zeros_like(p) for p in params]
  beta1, beta2, step = 0.9, 0.999, 0

  best, best_loss, best_epoch, stale = model.copy(), np.inf, 0, 0
  for epoch in range(1, epochs + 1):
    order = rng.permutation(len(Xt))
    for start in range(0, len(order), batch_size):
      batch = order[start:start + batch_size]
      _, gW, gb = model.loss_and_grads(Xt[batch], y_train[batch], w_train[batch], huber_delta)
      grads = [g + weight_decay * W for g, W in zip(gW, model.W, strict=True)] + gb
      step += 1
      for p, g, m_, v_ in zip(params, grads, m, v, strict=True):
        m_ *= beta1
        m_ += (1 - beta1) * g
        v_ *= beta2
        v_ += (1 - beta2) * g * g
        p -= lr * (m_ / (1 - beta1 ** step)) / (np.sqrt(v_ / (1 - beta2 ** step)) + 1e-8)

    val_loss = model.loss_and_grads(Xv, y_val, w_val, huber_delta)[0]
    if val_loss < best_loss * (1 - 1e-4):
      best, best_loss, best_epoch, stale = model.copy(), val_loss, epoch, 0
    else:
      stale += 1
      if stale % 3 == 0:
        lr *= 0.5
      if stale >= patience:
        break
    if verbose:
      print(f"  epoch {epoch:3d}  validation loss {val_loss:.6f}{'  *' if best_epoch == epoch else ''}")

  return TrainResult(best, mean, std, best_loss, best_epoch)


def predict(result: TrainResult, X: np.ndarray) -> np.ndarray:
  return result.model.predict((X - result.mean) / result.std)


def export_model(result: TrainResult, path: Path) -> dict:
  """Write the JSON layout FluxModel reads: weights stored (out, in), biases (out, 1)."""
  layers = []
  for k, (W, b) in enumerate(zip(result.model.W, result.model.b, strict=True)):
    layers.append({
      f"dense_{k + 1}_W": W.T.tolist(),
      f"dense_{k + 1}_b": b.reshape(-1, 1).tolist(),
      "activation": "identity" if k == len(result.model.W) - 1 else "σ",
    })
  model = {
    "input_size": len(INPUT_VARS),
    "output_size": 1,
    "input_vars": INPUT_VARS,
    "input_mean": result.mean.tolist(),
    "input_std": result.std.tolist(),
    "layers": layers,
    "model_test_loss": result.val_loss,
    "current_date_and_time": datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S"),
  }
  path.parent.mkdir(parents=True, exist_ok=True)
  with open(path, "w", encoding="utf-8") as f:
    json.dump(model, f, ensure_ascii=False)
  return model


# *** evaluation ***

def siglin(x: np.ndarray, a: float, b: float, c: float, d: float) -> np.ndarray:
  """Same curve as opendbc/car/mazda/interface.py's NON_LINEAR_TORQUE_PARAMS."""
  sig = np.sign(a * x) * (1 / (1 + np.exp(-np.abs(a * x))) - 0.5)
  return sig * b + x * c + d


def steady_state_rows(v_ego: float, lateral_accels, roll: float = 0.0, jerk: float = 0.0) -> np.ndarray:
  """Rows for holding a constant lateral accel: the shape of the curve the model learned."""
  la = np.asarray(lateral_accels, dtype=float)
  n_hist = len(PAST_TIMES) + len(FUTURE_TIMES)
  return np.column_stack([np.full_like(la, v_ego), la, np.full_like(la, jerk), np.full_like(la, roll)] +
                         [la] * n_hist + [np.full_like(la, roll)] * n_hist)


def friction_override_triggers(result: TrainResult) -> bool:
  # FluxModel.check_for_friction_override: evaluate([10.0, 0.0, 0.2]), the rest zero-filled
  row = np.zeros((1, len(INPUT_VARS)))
  row[0, :3] = [10.0, 0.0, 0.2]
  return bool(predict(result, row)[0] < 0.1)


def metrics(y: np.ndarray, pred: np.ndarray) -> dict[str, float]:
  err = pred - y
  return {"rmse": float(np.sqrt(np.mean(err ** 2))), "mae": float(np.mean(np.abs(err))),
          "r2": float(1 - np.sum(err ** 2) / max(np.sum((y - y.mean()) ** 2), 1e-12))}


def estimate_lag(routes: list[Route], max_lag: float = 1.0) -> float:
  """Where torque rate best lines up with lateral jerk; a cross-check on liveDelay, not used for training."""
  lags = np.arange(int(round(max_lag / DT)) + 1)
  num, den_a, den_b = np.zeros(len(lags)), np.zeros(len(lags)), np.zeros(len(lags))
  for r in routes:
    a = np.gradient(_moving_average(r.torque, int(round(JERK_SMOOTH / DT)) + 1), DT)
    b = r.lat_jerk
    for j, k in enumerate(lags):
      valid = r.ok[:len(r.ok) - k] & r.ok[k:]
      aa, bb = a[:len(a) - k][valid], b[k:][valid]
      num[j] += aa @ bb
      den_a[j] += aa @ aa
      den_b[j] += bb @ bb
  corr = num / np.sqrt(np.maximum(den_a * den_b, 1e-12))
  return float(lags[np.argmax(corr)] * DT)


# *** main ***

def load_routes(log_dirs, cache_dir, cereal_dir, workers) -> list[Route]:
  segments_by_route = find_segments(log_dirs)
  jobs = [(route, seg, path) for route, segs in segments_by_route.items() for seg, path in segs]
  if not jobs:
    raise SystemExit(f"no rlogs found under {', '.join(map(str, log_dirs))}")
  print(f"reading {len(jobs)} segments from {len(segments_by_route)} routes")

  extracted = defaultdict(dict)
  with ProcessPoolExecutor(max_workers=workers) as pool:
    futures = {pool.submit(extract_segment_cached, path, cache_dir, cereal_dir): (route, seg) for route, seg, path in jobs}
    for n, fut in enumerate(futures, 1):
      route, seg = futures[fut]
      try:
        extracted[route][seg] = fut.result()
      except Exception as e:
        print(f"warning: {route}--{seg}: {e!r}", file=sys.stderr)
      if n % 25 == 0 or n == len(futures):
        print(f"  {n}/{len(futures)} segments")

  routes = []
  for route, segs in extracted.items():
    r = assemble_route(route, [segs[k] for k in sorted(segs)])
    if r is not None:
      routes.append(r)
  return routes


def main(argv=None):
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("log_dirs", nargs="+", type=Path, help="folders containing <route>--<segment> folders with rlogs")
  parser.add_argument("--out", type=Path, help="output JSON (default: <fingerprint>.json in the current folder)")
  parser.add_argument("--fingerprint", help="only use routes from this car, e.g. MAZDA_CX_30")
  parser.add_argument("--lag", type=float, help="steering lag in seconds instead of each route's logged liveDelay")
  parser.add_argument("--min-speed", type=float, default=3.0, help="m/s, slower rows are dropped (default 3)")
  parser.add_argument("--stride", type=int, default=5, help="keep every Nth 10 ms row (default 5, i.e. 20 Hz)")
  parser.add_argument("--hidden", type=int, nargs="+", default=[24, 12], help="hidden layer sizes (default 24 12)")
  parser.add_argument("--epochs", type=int, default=60)
  parser.add_argument("--val-fraction", type=float, default=0.2, help="share of routes held out for validation")
  parser.add_argument("--no-mirror", action="store_true", help="don't add left/right mirrored rows")
  parser.add_argument("--siglin", type=float, nargs=4, metavar=("A", "B", "C", "D"),
                      help="your current siglin params, to compare against (see opendbc/car/mazda/interface.py)")
  parser.add_argument("--seed", type=int, default=0)
  parser.add_argument("--workers", type=int, default=max(1, (os.cpu_count() or 2) - 1))
  parser.add_argument("--cache", type=Path, default=Path.home() / ".cache" / "nnff_train",
                      help="where extracted segments are cached between runs")
  parser.add_argument("--no-cache", action="store_true")
  parser.add_argument("--cereal", type=Path, help="cereal schema folder (default: this repo's)")
  parser.add_argument("--dump", type=Path, help="also save the validation rows and predictions to this .npz")
  args = parser.parse_args(argv)

  routes = load_routes(args.log_dirs, None if args.no_cache else args.cache, args.cereal, args.workers)
  # carParams is only logged every ~50 s, so a short partial route may not name its car
  fingerprints = Counter(r.fingerprint for r in routes if r.fingerprint)
  if len(fingerprints) > 1 and not args.fingerprint:
    raise SystemExit(f"logs are from more than one car ({dict(fingerprints)}); pick one with --fingerprint")
  fingerprint = args.fingerprint or (fingerprints.most_common(1)[0][0] if fingerprints else None)
  if fingerprint is None:
    raise SystemExit("no carParams in these logs to tell which car they're from; pass --fingerprint")
  routes = [r for r in routes if r.fingerprint in (fingerprint, "")]
  if args.lag is None:
    for r in routes:
      if not np.all(np.isfinite(r.lag)):
        print(f"warning: route {r.name} logged no liveDelay or carParams, skipping it (or pass --lag)", file=sys.stderr)
    routes = [r for r in routes if np.all(np.isfinite(r.lag))]
  if not routes:
    raise SystemExit("no usable routes")

  if args.lag is None:
    lags = np.concatenate([r.lag[r.ok] for r in routes])
    lo, mid, hi = np.percentile(lags, [5, 50, 95]) if len(lags) else (np.nan,) * 3
    print(f"\n{fingerprint}: {len(routes)} routes, logged steering lag {mid:.3f} s (5-95%: {lo:.3f}-{hi:.3f}, it follows speed)")
  else:
    print(f"\n{fingerprint}: {len(routes)} routes, steering lag set to {args.lag:.3f} s")
  print(f"for reference, torque rate lines up best with lateral jerk {estimate_lag(routes):.2f} s later")

  per_route = [build_rows(r, args.lag, args.stride, args.min_speed) for r in routes]
  rng = np.random.default_rng(args.seed)
  if len(routes) > 1:
    n_val = max(1, int(round(len(routes) * args.val_fraction)))
    is_val = np.zeros(len(routes), dtype=bool)
    is_val[rng.permutation(len(routes))[:n_val]] = True
    X_train = np.concatenate([X for (X, _), v in zip(per_route, is_val, strict=True) if not v])
    y_train = np.concatenate([y for (_, y), v in zip(per_route, is_val, strict=True) if not v])
    X_val = np.concatenate([X for (X, _), v in zip(per_route, is_val, strict=True) if v])
    y_val = np.concatenate([y for (_, y), v in zip(per_route, is_val, strict=True) if v])
  else:
    # a single route can only hold out its own last fifth, which is weaker evidence
    n_val = 0
    X, y = per_route[0]
    cut = int(len(X) * 0.8)
    X_train, y_train, X_val, y_val = X[:cut], y[:cut], X[cut:], y[cut:]
    print("warning: only one route, so validation is its last fifth rather than unseen routes")
  if len(X_train) < 1000 or len(X_val) < 200:
    raise SystemExit(f"too little engaged driving: {len(X_train)} training rows, {len(X_val)} validation rows")

  hours = len(X_train) * args.stride * DT / 3600
  print(f"{len(X_train)} training rows (~{hours:.1f} h engaged), {len(X_val)} validation rows from {n_val} held-out routes")
  if np.corrcoef(X_train[:, 1], y_train)[0, 1] <= 0:
    print("warning: torque and lateral accel are not positively correlated; check the logs' sign convention")
  curvy = np.sum(np.abs(X_train[:, 1]) > 1.5)
  if curvy < 2000:
    print(f"warning: only {curvy} rows above 1.5 m/s^2 lateral accel; the model will be guessing in harder curves")

  if not args.no_mirror:
    X_train, y_train = mirror(X_train, y_train)
  w_train = balance_weights(X_train[:, 1])
  w_val = balance_weights(X_val[:, 1])

  print(f"\ntraining {len(INPUT_VARS)}-{'-'.join(map(str, args.hidden))}-1 network")
  result = train(X_train, y_train, w_train, X_val, y_val, w_val, hidden=tuple(args.hidden), epochs=args.epochs, seed=args.seed)

  pred_val = predict(result, X_val)
  gravity_adjusted = X_val[:, 1] - GRAVITY * X_val[:, 3]
  k = np.linalg.lstsq((X_train[:, 1] - GRAVITY * X_train[:, 3])[:, None], y_train, rcond=None)[0][0]
  candidates = {"NNFF": pred_val, "linear": k * gravity_adjusted}
  if args.siglin:
    candidates["siglin"] = siglin(gravity_adjusted, *args.siglin)

  print("\nheld-out routes, torque error (lower is better):")
  bins = [(0.0, 0.5), (0.5, 1.0), (1.0, 2.0), (2.0, np.inf)]
  bin_labels = [f"|la| {lo:.1f}-{hi:.1f}".replace("-inf", "+") for lo, hi in bins]
  print(f"  {'':8s} {'RMSE':>7s} {'MAE':>7s} {'R2':>6s}" + "".join(f"{label:>14s}" for label in bin_labels))
  la_abs = np.abs(X_val[:, 1])
  for name, pred in candidates.items():
    m = metrics(y_val, pred)
    per_bin = ""
    for lo, hi in bins:
      sel = (la_abs >= lo) & (la_abs < hi)
      per_bin += f"{metrics(y_val[sel], pred[sel])['rmse']:14.4f}" if sel.sum() > 50 else f"{'-':>14s}"
    print(f"  {name:8s} {m['rmse']:7.4f} {m['mae']:7.4f} {m['r2']:6.3f}" + per_bin)

  print("\nsteady-state torque the model learned (roll 0, no jerk):")
  grid = np.array([-3.0, -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0, 3.0])
  print("  lat accel  " + " ".join(f"{la:6.1f}" for la in grid))
  for v in (10.0, 20.0, 30.0):
    print(f"  {v:4.0f} m/s   " + " ".join(f"{t:6.3f}" for t in predict(result, steady_state_rows(v, grid))))
  if args.siglin:
    print("  siglin     " + " ".join(f"{t:6.3f}" for t in siglin(grid, *args.siglin)))
  if friction_override_triggers(result):
    print("\nNNFF friction override WILL be used with this model (its friction response is weak, so learned friction is added)")
  else:
    print("\nNNFF friction override will not be used with this model (friction comes from the network)")

  out = args.out or Path(f"{fingerprint}.json")
  export_model(result, out)
  print(f"\nwrote {out}  (best epoch {result.epochs}, validation loss {result.val_loss:.6f})")
  print(f"install: copy it to starpilot/assets/nnff_models/{fingerprint}.json, turn on NNFF, reboot")
  if args.dump:
    np.savez(args.dump, X_val=X_val, y_val=y_val, pred_val=pred_val, input_vars=np.array(INPUT_VARS))
    print(f"wrote {args.dump}")


if __name__ == "__main__":
  main()
