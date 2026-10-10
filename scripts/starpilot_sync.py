#!/usr/bin/env python3
"""
Mirror StarPilot into this repo without its Git LFS history.

StarPilot history before CUTOFF references ~23 GB of Git LFS objects stored on
comma's Hugging Face LFS server, not GitHub, so GitHub rejects any push that
contains it (GH008). CUTOFF is the commit where StarPilot moved everything into
plain git, so this script rewrites upstream history with CUTOFF as a root commit.

Only pre-CUTOFF parents and commit signatures are dropped; trees, authors,
dates and messages are byte-identical. The same upstream commit therefore
always maps to the same rewritten SHA, and each sync extends the mirror branch
instead of replacing it.

Usage:
  scripts/starpilot_sync.py             # fetch upstream, update the starpilot-upstream branch
  git rebase starpilot-upstream         # rebase your work onto it
  git push origin starpilot-upstream    # optional: publish the mirror
"""
import argparse
import hashlib
import subprocess
import sys
import tempfile
from pathlib import Path

UPSTREAM_URL = "https://github.com/firestar5683/StarPilot"
UPSTREAM_BRANCH = "SecretGoodStarPilot"
CUTOFF = "22a8575199bfb5676da392cdc2281ef8c06533e8"  # "Vendor dependencies and resources for standalone builds"
SHALLOW_SINCE = "2026-09-25"  # comfortably before CUTOFF, for shallow clones
LFS_POINTER = b"version https://git-lfs.github.com/spec/v1"


def git(*args: str, stdin: bytes | None = None) -> bytes:
  return subprocess.run(["git", *args], input=stdin, stdout=subprocess.PIPE, check=True).stdout


def has_object(rev: str) -> bool:
  return subprocess.run(["git", "cat-file", "-e", rev], stderr=subprocess.DEVNULL).returncode == 0


def read_commits(shas: list[str]) -> dict[str, bytes]:
  out = git("cat-file", "--batch", stdin="\n".join(shas).encode() + b"\n")
  commits, pos = {}, 0
  while pos < len(out):
    nl = out.index(b"\n", pos)
    sha, kind, size = out[pos:nl].split()
    assert kind == b"commit", (sha, kind)
    start = nl + 1
    commits[sha.decode()] = out[start:start + int(size)]
    pos = start + int(size) + 1
  return commits


def rewrite(raw: bytes, parents: list[str]) -> bytes:
  header, sep, message = raw.partition(b"\n\n")
  lines, in_signature = [], False
  for line in header.split(b"\n"):
    if line.startswith(b" "):  # continuation of a multi-line header
      if not in_signature:
        lines.append(line)
      continue
    key = line.split(b" ", 1)[0]
    in_signature = key in (b"gpgsig", b"gpgsig-sha256")
    if key == b"parent" or in_signature:
      continue
    lines.append(line)
    if key == b"tree":
      lines += [b"parent " + p.encode() for p in parents]
  return b"\n".join(lines) + sep + message


def main() -> int:
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("--remote", default="starpilot", help="remote name for StarPilot (added if missing)")
  parser.add_argument("--branch", default="starpilot-upstream", help="local branch that receives the rewritten mirror")
  parser.add_argument("--no-fetch", action="store_true", help="use the already fetched remote-tracking branch")
  args = parser.parse_args()

  upstream_ref = f"refs/remotes/{args.remote}/{UPSTREAM_BRANCH}"
  if not args.no_fetch:
    if args.remote not in git("remote").decode().split():
      git("remote", "add", args.remote, UPSTREAM_URL)
    shallow = git("rev-parse", "--is-shallow-repository").strip() == b"true"
    subprocess.run(["git", "fetch", *([f"--shallow-since={SHALLOW_SINCE}"] if shallow else []),
                    args.remote, f"+refs/heads/{UPSTREAM_BRANCH}:{upstream_ref}"], check=True)

  if not has_object(f"{CUTOFF}^{{commit}}"):
    sys.exit(f"cutoff {CUTOFF[:12]} is missing; fetch more StarPilot history (git fetch --deepen=1000 {args.remote})")

  tip = git("rev-parse", upstream_ref).decode().strip()
  cutoff_parents = [line.split()[1].decode() for line in read_commits([CUTOFF])[CUTOFF].split(b"\n\n")[0].split(b"\n")
                    if line.startswith(b"parent ")]
  exclude = [p for p in cutoff_parents if has_object(f"{p}^{{commit}}")]
  history = [line.split() for line in git("rev-list", "--topo-order", "--reverse", "--parents", tip,
                                          "--not", *exclude, "--").decode().splitlines()]
  if CUTOFF not in (c for c, *_ in history):
    sys.exit(f"{upstream_ref} does not contain cutoff {CUTOFF[:12]}; upstream history was rewritten, update CUTOFF")

  algo = git("rev-parse", "--show-object-format").decode().strip()
  raw = read_commits([c for c, *_ in history])
  rewritten: dict[str, str] = {}
  with tempfile.TemporaryDirectory() as tmp:
    paths = []
    for i, (commit, *parents) in enumerate(history):
      # parents outside the rewritten range are pre-cutoff history and get dropped
      new_raw = rewrite(raw[commit], [rewritten[p] for p in parents if p in rewritten])
      rewritten[commit] = hashlib.new(algo, b"commit %d\0" % len(new_raw) + new_raw).hexdigest()
      path = Path(tmp, str(i))
      path.write_bytes(new_raw)
      paths.append(str(path))
    written = git("hash-object", "-t", "commit", "-w", "--no-filters", "--stdin-paths",
                  stdin="\n".join(paths).encode() + b"\n").decode().split()
  assert written == [rewritten[c] for c, *_ in history], "rewritten commit hashes do not match"

  new_tip = rewritten[tip]
  branch_ref = f"refs/heads/{args.branch}"
  old_tip = git("rev-parse", "--verify", "--quiet", branch_ref).decode().strip() if has_object(branch_ref) else ""

  # GitHub rejects pushes that reference LFS objects it does not store
  objects = git("rev-list", "--objects", "--no-object-names", new_tip, *(["--not", old_tip] if old_tip else []))
  sizes = git("cat-file", "--batch-check=%(objecttype) %(objectname) %(objectsize)", stdin=objects).decode().split("\n")
  small_blobs = [s.split()[1] for s in sizes if s.startswith("blob ") and int(s.split()[2]) < 1024]
  if small_blobs:
    contents = git("cat-file", "--batch", stdin="\n".join(small_blobs).encode() + b"\n")
    if LFS_POINTER in contents:
      sys.exit("new upstream commits contain Git LFS pointers; GitHub would reject them, so the branch was not updated")

  git("update-ref", branch_ref, new_tip, *([old_tip] if old_tip else []))
  print(f"{args.branch}: {old_tip[:12] or '(new)'} -> {new_tip[:12]} (upstream {tip[:12]}, {len(history)} commits)")
  if old_tip and old_tip != new_tip and subprocess.run(["git", "merge-base", "--is-ancestor", old_tip, new_tip]).returncode != 0:
    print(f"warning: not a fast-forward; rebase with: git rebase --onto {args.branch} {old_tip[:12]}")
  return 0


if __name__ == "__main__":
  sys.exit(main())
