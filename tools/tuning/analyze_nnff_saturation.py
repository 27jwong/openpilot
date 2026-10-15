#!/usr/bin/env python3
"""Find out why "Turn Exceeds Steering Limit" fires, from your own rlogs.

selfdrived raises that alert when all of these hold (selfdrive/selfdrived/selfdrived.py): the car is
getting under 1/1.2 of the lateral acceleration the model wants, the model wants more than 1 m/s^2,
and the controller is at (nearly) full torque or its saturation timer has run out. So at every alert
the controller was asking for about as much torque as it has and the car still wasn't turning enough.

This compares routes steered by the stock torque controller with routes steered by NNFF, turn by turn:
how much torque each commanded for the lateral acceleration wanted, how closely the car followed, and
what the static siglin curve says for the same spot. Both write the same log fields, but the
stock controller's p, i and f are in m/s^2 and NNFF's in torque, so only the final output (torque) and
the lateral accelerations are compared across them.

  python tools/tuning/analyze_nnff_saturation.py D:/logs/realdata --from-route 000002a0

--from-route is the hex route counter to start at (route names count up in time), to skip old drives.
Needs what train_nnff.py needs: numpy, pycapnp, zstandard and this repo's cereal.
"""
import argparse
import sys
from collections import defaultdict
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import train_nnff as T  # noqa: E402

# selfdrived's conditions for the alert
UNDERSHOOT_RATIO = 1.2
TURNING_LAT_ACCEL = 1.0
AT_MAX_TORQUE = 0.99
ALERT_NAME = "steerSaturated"

BINS = [(1.0, 1.5), (1.5, 2.0), (2.0, 2.5), (2.5, np.inf)]
KEYS = ("t", "desired", "actual", "f", "p", "i", "output", "saturated", "active", "version")


def controller_name(version: float) -> str:
  """LatControlTorque stamps its log version (2 at the time of writing); LatControlNNFF leaves it 0."""
  return "NNFF" if version == 0 else "torque"


def extract_segment(path: Path, cereal_dir: Path | None = None) -> dict[str, np.ndarray]:
  log = T.load_log_schema(cereal_dir)
  cols = defaultdict(list)
  alerts = []
  try:
    for evt in log.Event.read_multiple_bytes(T.read_log_bytes(path)):
      which = evt.which()
      t = evt.logMonoTime * 1e-9
      if which == "controlsState":
        lcs = evt.controlsState.lateralControlState
        if lcs.which() != "torqueState":
          continue
        s = lcs.torqueState
        row = (t, s.desiredLateralAccel, s.actualLateralAccel, s.f, s.p, s.i, s.output, s.saturated, s.active, s.version)
        for k, v in zip(KEYS, row, strict=True):
          cols[k].append(v)
      elif which == "carState":
        cs = evt.carState
        for k, v in (("cs_t", t), ("v_ego", cs.vEgo), ("pressed", cs.steeringPressed)):
          cols[k].append(v)
      elif which == "onroadEvents":
        if any(str(e.name) == ALERT_NAME for e in evt.onroadEvents):
          alerts.append(t)
  except AttributeError:
    raise
  except Exception as e:  # a truncated final message
    print(f"warning: {path}: stopped reading at {e!r}", file=sys.stderr)
  out = {k: np.asarray(v, dtype=np.float64) for k, v in cols.items()}
  out["alerts"] = np.asarray(alerts, dtype=np.float64)
  return out


def assemble(segments: list[dict[str, np.ndarray]]) -> dict[str, np.ndarray] | None:
  cat = {k: np.concatenate([np.zeros(0)] + [s[k] for s in segments if k in s]) for k in (*KEYS, "cs_t", "v_ego", "pressed", "alerts")}
  if len(cat["t"]) < 2 or len(cat["cs_t"]) < 2:
    return None
  order = np.argsort(cat["t"], kind="stable")
  d = {k: cat[k][order] for k in KEYS}
  c_order = np.argsort(cat["cs_t"], kind="stable")
  cs_t = cat["cs_t"][c_order]
  d["v_ego"] = np.interp(d["t"], cs_t, cat["v_ego"][c_order])
  d["pressed"] = cat["pressed"][c_order][np.clip(np.searchsorted(cs_t, d["t"], side="right") - 1, 0, len(cs_t) - 1)]
  d["alerts"] = np.sort(cat["alerts"])
  return d


def analyze(d: dict[str, np.ndarray], siglin_params: tuple[float, ...]) -> dict:
  """Rows are controlsState messages while openpilot steers untouched. 'would alert' is selfdrived's test
  applied to them; 'bins' pool the turning rows by how much lateral acceleration was wanted. Torques are
  magnitudes (the logged output has the car's sign, opposite to the lateral acceleration's)."""
  dt = float(np.median(np.diff(d["t"]))) if len(d["t"]) > 2 else T.DT
  ok = (d["active"] > 0.5) & (d["pressed"] < 0.5) & (d["v_ego"] > 1.0)
  des, act, out = d["desired"], d["actual"], np.abs(d["output"])
  under = np.abs(des) / (np.abs(act) + 1e-3) > UNDERSHOOT_RATIO
  turning = np.abs(des) > TURNING_LAT_ACCEL
  at_max = (out > AT_MAX_TORQUE) | (d["saturated"] > 0.5)
  sign = np.sign(des)
  siglin_torque = np.abs(T.siglin(des, *siglin_params))  # what the old curve asks for at this lateral acceleration
  bins = []
  for lo, hi in BINS:
    m = ok & turning & (np.abs(des) >= lo) & (np.abs(des) < hi)
    n = int(m.sum())
    mean = lambda x: float(x[m].mean()) if n else np.nan  # noqa: E731
    bins.append(dict(lo=lo, hi=hi, seconds=n * dt, undershoot=mean(under), at_max=mean(at_max), output=mean(out), siglin=mean(siglin_torque),
                     track=mean(sign * act / np.where(np.abs(des) > 0, np.abs(des), 1.0))))
  hard = ok & turning & (np.abs(des) >= 1.5)
  alerts = []
  for ta in d["alerts"]:
    k = int(np.argmin(np.abs(d["t"] - ta)))
    alerts.append(dict(v_ego=float(d["v_ego"][k]), desired=float(des[k]), actual=float(act[k]), output=float(out[k]), siglin=float(siglin_torque[k]),
                       f=float(sign[k] * d["f"][k]), p=float(sign[k] * d["p"][k]), i=float(sign[k] * d["i"][k])))
  versions = d["version"][ok]
  controller = controller_name(float(np.median(versions))) if len(versions) else "?"
  return dict(controller=controller, engaged=float(ok.sum() * dt), would_alert=float((ok & under & turning & at_max).sum() * dt), alerts=alerts,
              bins=bins, out_gap=float(np.mean((out - siglin_torque)[hard])) if hard.any() else np.nan)


def route_counter(route: str) -> int:
  """The hex counter that starts a route name, -1 for the older date-style names."""
  try:
    return int(route.split("--")[0], 16)
  except ValueError:
    return -1


def load(log_dirs, from_route: int | None, cereal_dir, workers) -> dict[str, dict]:
  segments = T.find_segments(log_dirs)
  if from_route is not None:
    segments = {r: s for r, s in segments.items() if route_counter(r) >= from_route}
  jobs = [(r, seg, p) for r, segs in segments.items() for seg, p in segs]
  if not jobs:
    raise SystemExit("no rlogs found" + (" at or after that route" if from_route is not None else ""))
  print(f"reading {len(jobs)} segments from {len(segments)} routes")
  extracted = defaultdict(list)
  with ProcessPoolExecutor(max_workers=workers) as pool:
    for (route, _, _), fut in zip(jobs, [pool.submit(extract_segment, p, cereal_dir) for _, _, p in jobs], strict=True):
      try:
        extracted[route].append(fut.result())
      except AttributeError as e:
        raise SystemExit(f"the logs hold a field the cereal schema in use doesn't have ({e}); use a cereal from the same "
                         "openpilot version as the logs (run from an up-to-date checkout, without --cereal)") from e
  routes = {}
  for route, segs in extracted.items():
    d = assemble(segs)
    if d is not None:
      routes[route] = d
  return routes


def report(results: dict[str, dict]) -> None:
  print(f"\n{'route':24s} {'steered by':>10s} {'min':>6s} {'alerts':>7s} {'would-alert s':>14s} {'torque - siglin at 1.5+':>24s}")
  for r, a in results.items():
    print(f"{r:24s} {a['controller']:>10s} {a['engaged'] / 60:6.1f} {len(a['alerts']):7d} {a['would_alert']:14.1f} {a['out_gap']:24.3f}")

  for name in ("torque", "NNFF"):
    mine = [a for a in results.values() if a["controller"] == name]
    if not mine:
      continue
    print(f"\n{name} controller, turning rows pooled by |lateral acceleration wanted| ({sum(a['engaged'] for a in mine) / 3600:.1f} h):")
    print(f"  {'m/s^2':9s} {'seconds':>8s} {'undershoot':>11s} {'at max':>7s} {'torque':>7s} {'siglin':>7s} {'car got':>8s}")
    for k, (lo, hi) in enumerate(BINS):
      rows = [a["bins"][k] for a in mine if a["bins"][k]["seconds"] > 0]
      secs = sum(b["seconds"] for b in rows)
      if not secs:
        continue
      w = lambda key: sum(b[key] * b["seconds"] for b in rows) / secs  # noqa: E731
      label = f"{lo:.1f}-{hi:.1f}" if np.isfinite(hi) else f"{lo:.1f}+"
      print(f"  {label:9s} {secs:8.1f} {w('undershoot'):11.0%} {w('at_max'):7.0%} {w('output'):7.2f} {w('siglin'):7.2f} {w('track'):8.0%}")
  print("  (torque = mean torque the controller commanded; car got = lateral acceleration achieved as a share of wanted;")
  print("   siglin = the static default curve, not the live-learned parameters the torque controller really used)")

  alerts = [(r, a["controller"], al) for r, a in results.items() for al in a["alerts"]]
  if alerts:
    print(f"\n{len(alerts)} logged alerts; first {min(len(alerts), 12)} (p, i, f are m/s^2 for the torque controller, torque for NNFF):")
    print(f"  {'route':24s} {'by':>6s} {'v m/s':>6s} {'wanted':>7s} {'got':>6s} {'torque':>7s} {'siglin':>7s} {'p':>6s} {'i':>6s} {'f':>6s}")
    for r, c, al in alerts[:12]:
      print(f"  {r:24s} {c:>6s} {al['v_ego']:6.1f} {al['desired']:7.2f} {al['actual']:6.2f} {al['output']:7.2f} {al['siglin']:7.2f} "
            f"{al['p']:6.2f} {al['i']:6.2f} {al['f']:6.2f}")
  else:
    print("\nno 'Turn Exceeds Steering Limit' alerts were logged in these routes")


def main(argv=None):
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("log_dirs", nargs="+", type=Path)
  parser.add_argument("--from-route", help="hex route counter to start at (e.g. 000002a0)")
  parser.add_argument("--siglin", type=float, nargs=4, metavar=("A", "B", "C", "D"), default=(6.69417, 0.71168, 0.21504, 0.02331),
                      help="the torque curve to compare with (default: the CX-30's, from opendbc/car/mazda/interface.py)")
  parser.add_argument("--cereal", type=Path, help="cereal schema folder (default: this repo's)")
  parser.add_argument("--workers", type=int, default=4)
  args = parser.parse_args(argv)

  T.check_schema(args.cereal)
  routes = load(args.log_dirs, int(args.from_route, 16) if args.from_route else None, args.cereal, args.workers)
  results = {r: analyze(d, tuple(args.siglin)) for r, d in sorted(routes.items())}
  results = {r: a for r, a in results.items() if a["engaged"] > 0}
  if not results:
    raise SystemExit("none of these routes were steering with the torque controller or NNFF")
  report(results)


if __name__ == "__main__":
  main()
