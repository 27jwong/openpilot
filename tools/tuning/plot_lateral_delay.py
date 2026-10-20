#!/usr/bin/env python3
"""Plot the steering delay lagd has learned, against vehicle speed.

lagd keeps one delay estimate per speed bin (selfdrive/locationd/lagd.py) and logs all of them in liveDelay.speedBins.
This takes the last liveDelay of each route and draws its curve: the delay controlsd applied at each bin's anchor speed
(a filled dot once the bin has enough valid blocks of its own, a hollow one while it is still blended toward the
all-speed value), the raw estimate +- std next to it, and below that how many valid blocks each bin has learned.
The values are the full lateral delay, so they include the 0.2 s of control software delay on top of the actuator's.

  python tools/tuning/plot_lateral_delay.py D:/logs/realdata                 # the last 6 routes
  python tools/tuning/plot_lateral_delay.py D:/logs/realdata --last 1 --kph
  python tools/tuning/plot_lateral_delay.py LiveDelay                        # the param file, from /data/params/d/ on the device
  python tools/tuning/plot_lateral_delay.py D:/logs/realdata --out delay.png

Folders are searched for <route>--<segment>/rlog* and a lone rlog or param file is read as it is. Needs what
train_nnff.py needs (numpy, pycapnp, zstandard, this repo's cereal) plus matplotlib. On Windows, where git
checks the cereal symlinks out as text stubs, it reads a resolved copy of the schemas from train_nnff's cache folder.
"""
import argparse
import sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import train_nnff as T  # noqa: E402

MIN_VALID_BLOCKS = 5  # lagd.BLOCK_NUM_NEEDED: blocks a bin needs before it stands on its own
UNITS = {"mph": 2.23694, "kph": 3.6}


def last_live_delay(paths: list[Path], cereal_dir: Path | None = None) -> dict | None:
  """The final liveDelay of the newest log in paths (newest first) that has one, as plain numbers."""
  log = T.load_log_schema(cereal_dir)
  for path in paths:
    last = None
    try:
      for evt in log.Event.read_multiple_bytes(T.read_log_bytes(path)):
        if evt.which() == "liveDelay":
          ld = evt.liveDelay
          last = {"delay": ld.lateralDelay, "estimate": ld.lateralDelayEstimate, "blocks": ld.validBlocks, "status": str(ld.status),
                  "bins": [(b.speed, b.lateralDelay, b.lateralDelayEstimate, b.lateralDelayEstimateStd, b.validBlocks, str(b.status))
                           for b in ld.speedBins]}
    except Exception as e:  # a truncated final message
      print(f"warning: {path}: stopped reading at {e!r}", file=sys.stderr)
    if last is not None:
      return last
  return None


def collect(inputs: list[Path], last: int) -> list[tuple[str, list[Path]]]:
  """(label, logs newest first) for each route found in folders, and for each file given directly."""
  routes, files = [], []
  for p in inputs:
    if p.is_dir():
      found = T.find_segments([p])
      if not found:
        raise SystemExit(f"no <route>--<segment>/rlog* found under {p}")
      routes += [(name, [path for _, path in reversed(segs)]) for name, segs in found.items()]
    elif p.is_file():
      files.append((p.parent.name if p.name.startswith("rlog") else p.name, [p]))
    else:
      raise SystemExit(f"{p} doesn't exist")
  routes.sort(key=lambda r: r[0])  # route names count up in time
  return (routes[-last:] if last > 0 else routes) + files


def plot(records: list[tuple[str, dict]], unit: str, out: Path | None) -> None:
  import matplotlib
  if out is not None:
    matplotlib.use("Agg")
  import matplotlib.pyplot as plt

  k = UNITS[unit]
  n = len(records)
  speeds = np.array([b[0] for b in records[0][1]["bins"]]) * k
  group = 0.6 * float(np.min(np.diff(speeds))) / n if len(speeds) > 1 else 1.0  # width of one route's bar in a bin's cluster
  fig, (ax, bx) = plt.subplots(2, 1, sharex=True, figsize=(9, 6.5), gridspec_kw={"height_ratios": (3, 1)})
  for i, (label, rec) in enumerate(records):
    bins = np.array([b[:5] for b in rec["bins"]], dtype=float)
    learned = np.array([b[5] == "estimated" for b in rec["bins"]])
    x, color, dx = bins[:, 0] * k, f"C{i % 10}", (i - (n - 1) / 2) * group
    ax.plot(x, bins[:, 1], "-", color=color, lw=1.5, label=label)
    ax.scatter(x[learned], bins[learned, 1], color=color, s=40, zorder=3)
    ax.scatter(x[~learned], bins[~learned, 1], facecolors="none", edgecolors=color, s=40, zorder=3)
    ax.errorbar(x + dx, bins[:, 2], yerr=bins[:, 3], fmt="x", color=color, alpha=0.4, capsize=2, ms=4, lw=1)
    bx.bar(x + dx, bins[:, 4], width=group, color=color, alpha=0.8)
  bx.axhline(MIN_VALID_BLOCKS, color="k", ls="--", lw=0.8)
  bx.annotate("learned", (1, MIN_VALID_BLOCKS), xycoords=("axes fraction", "data"), ha="right", va="bottom", fontsize=8)
  ax.set_ylabel("learned lateral delay (s)")
  ax.set_title("Learned steer delay vs speed")
  ax.grid(alpha=0.3)
  ax.legend(fontsize=8, loc="best", title="route", title_fontsize=8)
  ax.text(0.99, 0.02, "line: applied   hollow: still blended toward the all-speed value   x: raw estimate +- std",
          transform=ax.transAxes, ha="right", va="bottom", fontsize=7, alpha=0.7)
  bx.set_ylabel("valid blocks")
  bx.set_xlabel(f"speed ({unit})")
  bx.set_xticks(speeds)
  bx.set_xticklabels([f"{s:.0f}" for s in speeds])
  bx.grid(alpha=0.3)
  other = "kph" if unit == "mph" else "mph"
  top = ax.secondary_xaxis("top", functions=(lambda s: s * UNITS[other] / k, lambda s: s * k / UNITS[other]))
  top.set_xlabel(other, fontsize=8)
  fig.tight_layout()
  if out is not None:
    fig.savefig(out, dpi=130)
    print(f"wrote {out}")
  else:
    plt.show()


def main(argv=None):
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("inputs", nargs="+", type=Path, help="folders of route segments, rlog files, or a LiveDelay param file")
  parser.add_argument("--last", type=int, default=6, help="only the newest N routes found in folders (0 = all, default 6)")
  parser.add_argument("--kph", action="store_true", help="speeds in km/h instead of mph")
  parser.add_argument("--out", type=Path, help="save the figure here instead of opening a window")
  parser.add_argument("--cereal", type=Path, help="cereal schema folder (default: this repo's)")
  parser.add_argument("--workers", type=int, default=4)
  args = parser.parse_args(argv)

  cereal_dir = T.check_schema(args.cereal)  # a copy with symlink stubs resolved, if git left any
  jobs = collect(args.inputs, args.last)
  with ProcessPoolExecutor(max_workers=max(1, min(args.workers, len(jobs)))) as pool:
    results = list(pool.map(last_live_delay, [paths for _, paths in jobs], [cereal_dir] * len(jobs)))

  records = []
  for (label, _), rec in zip(jobs, results, strict=True):
    if rec is None:
      print(f"{label}: no liveDelay logged", file=sys.stderr)
    elif not rec["bins"]:
      print(f"{label}: no per-speed bins in its liveDelay (logged before per-speed learning); all-speed delay {rec['delay']:.3f} s", file=sys.stderr)
    else:
      records.append((label, rec))
  if not records:
    raise SystemExit("nothing to plot")

  unit = "kph" if args.kph else "mph"
  for label, rec in records:
    print(f"\n{label}: all-speed estimate {rec['estimate']:.3f} s ({rec['blocks']} blocks, {rec['status']})")
    print(f"  {unit:>5} {'applied':>8} {'estimate':>9} {'std':>6} {'blocks':>7}  status")
    for speed, delay, est, std, blocks, status in rec["bins"]:
      print(f"  {speed * UNITS[unit]:5.0f} {delay:8.3f} {est:9.3f} {std:6.3f} {blocks:7d}  {status}")
  plot(records, unit, args.out)


if __name__ == "__main__":
  main()
