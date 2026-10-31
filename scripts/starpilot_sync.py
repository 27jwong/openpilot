#!/usr/bin/env python3
"""
Mirror StarPilot into this repo without its Git LFS history.

StarPilot history before CUTOFF references ~23 GB of Git LFS objects stored on
comma's Hugging Face LFS server, not GitHub, so GitHub rejects any push that
contains it (GH008). CUTOFF is the commit where StarPilot moved everything into
plain git, so this script rewrites upstream history with CUTOFF as a root commit.

Only pre-CUTOFF parents and commit signatures are dropped; authors, dates and
messages are byte-identical, and so are trees, with one exception: upstream
sometimes commits a file as a Git LFS pointer for a few commits before fixing
it, and GitHub rejects pushes that reference LFS objects it does not store. In
those commits the pointer is replaced by the blob the file had in the first
parent (see lfs_fixups). The same upstream commit therefore always maps to the
same rewritten SHA, and each sync extends the mirror branch instead of
replacing it.

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

UPSTREAM_URL = "https://github.com/StarPilotHQ/StarPilot"
UPSTREAM_BRANCH = "Dom"  # SecretGoodStarPilot until 2026-10-28; Dom continues its history
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


def unpublished_pointers(tip: str, old_tip: str, exclude: list[str]) -> set[str]:
  """Git LFS pointer blobs reachable from tip that the mirror does not already contain."""
  objects = git("rev-list", "--objects", "--no-object-names", tip, "--not", *([old_tip] if old_tip else []), *exclude, "--")
  sizes = git("cat-file", "--batch-check=%(objecttype) %(objectname) %(objectsize)", stdin=objects).decode().splitlines()
  small = [s.split()[1] for s in sizes if s.startswith("blob ") and int(s.split()[2]) < 1024]
  if not small:
    return set()
  out = git("cat-file", "--batch", stdin="\n".join(small).encode() + b"\n")
  pointers, pos = set(), 0
  while pos < len(out):
    nl = out.index(b"\n", pos)
    sha, _, size = out[pos:nl].split()
    start = nl + 1
    if LFS_POINTER in out[start:start + int(size)]:
      pointers.add(sha.decode())
    pos = start + int(size) + 1
  return pointers


def tree_entry(tree: str, path: str) -> tuple[str, str] | None:
  """(mode, sha) of path in tree, or None."""
  out = git("ls-tree", "-z", tree, "--", path)
  if not out:
    return None
  meta = out.split(b"\t", 1)[0].decode().split()
  return meta[0], meta[2]


def set_blob(tree: str, parts: list[str], mode: str, blob: str) -> str:
  """tree with the entry at parts replaced by (mode, blob), rebuilding the trees above it."""
  out = []
  for e in git("ls-tree", "-z", tree).split(b"\x00"):
    if not e:
      continue
    meta, name = e.split(b"\t", 1)
    emode, etype, esha = meta.decode().split()
    if name.decode() == parts[0]:
      if len(parts) == 1:
        e = f"{mode} blob {blob}".encode() + b"\t" + name
      else:
        assert etype == "tree", (parts, etype)
        e = f"{emode} tree {set_blob(esha, parts[1:], mode, blob)}".encode() + b"\t" + name
    out.append(e)
  return git("mktree", "-z", stdin=b"\x00".join(out) + b"\x00").decode().strip()


def lfs_fixup(commit: str, parents: list[str], pointers: set[str], live: dict[str, dict[str, tuple[str, str]]]) -> str | None:
  """Tree of commit with each pointer in `pointers` swapped for the blob its path had before the pointer, or None.

  live maps commit -> {path: (mode, replacement blob)} along the first-parent chain, so a pointer is
  replaced where it appears and again in every later commit that inherits it, until upstream replaces the file.
  """
  inherited = dict(live.get(parents[0], {})) if parents else {}
  if parents and (inherited or pointers):
    changed = git("diff-tree", "-r", "-z", "--no-commit-id", "--raw", parents[0], commit).split(b"\x00")
    for i in range(0, len(changed) - 1, 2):
      _, mode, _, sha, status = changed[i].decode().split()
      path = changed[i + 1].decode()
      previous = inherited.pop(path, None)
      if status[0] == "D" or sha not in pointers:
        continue
      old = tree_entry(parents[0], path)
      if previous is not None:
        inherited[path] = (mode, previous[1])
      elif old is not None and old[1] not in pointers:
        inherited[path] = (mode, old[1])
      else:
        sys.exit(f"{path} is a Git LFS pointer in {commit[:12]} with no real version before it")
  live[commit] = inherited
  if not inherited:
    return None
  tree = git("rev-parse", f"{commit}^{{tree}}").decode().strip()
  for path, (mode, blob) in inherited.items():
    tree = set_blob(tree, path.split("/"), mode, blob)
  return tree


def rewrite(raw: bytes, parents: list[str], tree: str | None = None) -> bytes:
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
    lines.append(b"tree " + tree.encode() if key == b"tree" and tree else line)
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
  branch_ref = f"refs/heads/{args.branch}"
  old_tip = git("rev-parse", "--verify", "--quiet", branch_ref).decode().strip() if has_object(branch_ref) else ""
  known = set(git("rev-list", old_tip).decode().split()) if old_tip else set()  # commits already in the mirror
  pointers = unpublished_pointers(tip, old_tip, exclude)
  live: dict[str, dict[str, tuple[str, str]]] = {}

  raw = read_commits([c for c, *_ in history])
  rewritten: dict[str, str] = {}

  def commit_sha(new_raw: bytes) -> str:
    return hashlib.new(algo, b"commit %d\0" % len(new_raw) + new_raw).hexdigest()

  with tempfile.TemporaryDirectory() as tmp:
    paths = []
    for i, (commit, *parents) in enumerate(history):
      # parents outside the rewritten range are pre-cutoff history and get dropped
      kept = [rewritten[p] for p in parents if p in rewritten]
      new_raw = rewrite(raw[commit], kept)
      if commit_sha(new_raw) not in known:
        tree = lfs_fixup(commit, parents, pointers, live)
        if tree:
          new_raw = rewrite(raw[commit], kept, tree)
      rewritten[commit] = commit_sha(new_raw)
      path = Path(tmp, str(i))
      path.write_bytes(new_raw)
      paths.append(str(path))
    written = git("hash-object", "-t", "commit", "-w", "--no-filters", "--stdin-paths",
                  stdin="\n".join(paths).encode() + b"\n").decode().split()
  assert written == [rewritten[c] for c, *_ in history], "rewritten commit hashes do not match"

  new_tip = rewritten[tip]
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
