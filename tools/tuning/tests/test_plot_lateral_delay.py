import os
import shutil
import subprocess
import sys
from pathlib import Path

import zstandard

from cereal import log

TOOLS = Path(__file__).resolve().parents[1]
REPO_CEREAL = TOOLS.parents[1] / "cereal"


def stubbed_cereal(tmp_path):
  """This repo's cereal as Windows git leaves it: car.capnp is a text file naming opendbc's."""
  cereal, real = tmp_path / "repo" / "cereal", tmp_path / "repo" / "opendbc_repo"
  cereal.mkdir(parents=True)
  real.mkdir()
  for src in REPO_CEREAL.glob("*.capnp"):
    shutil.copyfile(src, cereal / src.name)
  shutil.copytree(REPO_CEREAL / "include", cereal / "include")
  (real / "car.capnp").write_bytes((cereal / "car.capnp").read_bytes())
  (cereal / "car.capnp").write_text("../opendbc_repo/car.capnp")
  return cereal


def write_rlog(path):
  evt = log.Event.new_message(logMonoTime=1, valid=True)
  ld = evt.init("liveDelay")
  ld.lateralDelay = 0.4
  ld.lateralDelayEstimate = 0.38
  ld.validBlocks = 12
  ld.status = "estimated"
  for b, (speed, blocks) in zip(ld.init("speedBins", 2), ((5.0, 2), (20.0, 9)), strict=True):
    b.speed = speed
    b.lateralDelay = 0.35
    b.lateralDelayEstimate = 0.36
    b.lateralDelayEstimateStd = 0.02
    b.validBlocks = blocks
    b.status = "estimated" if blocks >= 5 else "unestimated"
  path.write_bytes(zstandard.ZstdCompressor().compress(evt.to_bytes()))


def test_logs_are_read_with_a_resolved_copy_of_cereal_when_git_left_symlink_stubs(tmp_path):
  # capnp aborts the worker process on a stub rather than raising, so the workers must be handed the copy. The
  # script runs on its own because a process that has loaded this repo's cereal can't also load a copy of it.
  cereal = stubbed_cereal(tmp_path)
  rlog, out = tmp_path / "rlog.zst", tmp_path / "delay.png"
  write_rlog(rlog)
  home = {"HOME": str(tmp_path / "home"), "USERPROFILE": str(tmp_path / "home")}  # where the resolved copy is cached

  run = subprocess.run([sys.executable, str(TOOLS / "plot_lateral_delay.py"), str(rlog), "--cereal", str(cereal),
                        "--out", str(out), "--workers", "1"],
                       capture_output=True, text=True, timeout=300, env={**os.environ, **home})

  assert run.returncode == 0, run.stdout + run.stderr
  assert out.stat().st_size > 0
  assert " 45 " in run.stdout  # the 20 m/s bin (45 mph), read from the log
  assert (cereal / "car.capnp").read_text() == "../opendbc_repo/car.capnp"  # the checkout is left alone
  assert list((tmp_path / "home" / ".cache" / "nnff_train" / "schemas").glob("*/car.capnp"))
