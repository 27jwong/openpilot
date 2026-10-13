#!/usr/bin/env python3
"""Copy just the rlogs off your device, so train_nnff.py can learn from every drive you've ever kept.

The device deletes its oldest drives once storage runs low, and the camera video is nearly all of what
fills it. This copies only each segment's rlog (not the video) to your PC and never deletes anything on
the device, so run it whenever you're home and your PC ends up holding every drive you've synced.

It needs only Python 3.10+ and the OpenSSH client that ships with Windows 10/11, macOS and Linux:

  python tools/tuning/sync_rlogs.py 192.168.1.50 D:/logs/realdata
  python tools/tuning/sync_rlogs.py comma@192.168.1.50 D:/logs/realdata --dry-run

Then train from the same folder:

  python tools/tuning/train_nnff.py D:/logs/realdata

The device's SSH must be on (Settings > Developer), with your public key added. Segments already on
your PC with the same size are skipped, so reruns only fetch what's new, and an interrupted run
resumes where it stopped. The segment being recorded right now (it holds an rlog.lock) is skipped, so
copy while the car is off.

Folders are saved flat as <dest>/<route>--<segment>/rlog.zst, the layout train_nnff.py reads, whichever
of the device's realdata, realdata_HD or realdata_konik folders they came from.
"""
import argparse
import re
import shutil
import subprocess
import sys
import tarfile
import threading
from pathlib import Path

DEVICE_BASE = "/data/media/0"
DEVICE_ROOTS = ("realdata", "realdata_HD", "realdata_konik")  # which one is live depends on the server setting
RLOG_NAMES = ("rlog.zst", "rlog.bz2", "rlog")
ROOT_RE = re.compile(r"^[A-Za-z0-9_.-]+$")
SEGMENT_RE = re.compile(r"^.+--\d+$")


def ssh_command(ssh: str, host: str, port: int | None, identity: str | None, remote: str) -> list[str]:
  cmd = [ssh, "-o", "BatchMode=yes", "-o", "ConnectTimeout=10", "-o", "ServerAliveInterval=15",
         "-o", "StrictHostKeyChecking=accept-new"]
  if port is not None:
    cmd += ["-p", str(port)]
  if identity is not None:
    cmd += ["-i", identity]
  return cmd + [host, remote]


def list_device_rlogs(ssh_cmd, base: str, roots: tuple[str, ...]) -> tuple[dict[str, int], int]:
  """{<root>/<segment>/<rlog name>: size} for every finished rlog on the device, and how many segments
  were skipped because they're still being written."""
  remote = (f"cd {base} || exit 3; find {' '.join(roots)} -mindepth 2 -maxdepth 2 "
            r"\( -name 'rlog*' -o -name '*.lock' \) -printf '%p\t%s\n' 2>/dev/null; exit 0")
  proc = subprocess.run(ssh_cmd(remote), capture_output=True, text=True)
  if proc.returncode == 255:
    raise SystemExit(f"couldn't SSH to the device:\n{proc.stderr.strip()}\n"
                     "Check the IP, that SSH is enabled in Settings > Developer, and that your key is added.")
  if proc.returncode != 0:
    raise SystemExit(f"{base} isn't on the device (exit {proc.returncode}): {proc.stderr.strip()}")

  files: dict[str, int] = {}
  locked: set[str] = set()
  for line in proc.stdout.splitlines():
    path, _, size = line.partition("\t")
    parts = path.split("/")
    if len(parts) != 3 or not SEGMENT_RE.match(parts[1]):
      continue
    if parts[2].endswith(".lock"):
      locked.add(parts[1])
    elif parts[2] in RLOG_NAMES and size.isdigit():
      files[path] = int(size)

  recording = {p for p in files if p.split("/")[1] in locked}
  return {p: s for p, s in files.items() if p not in recording}, len(recording)


def local_path(dest: Path, device_path: str) -> Path:
  _, segment, name = device_path.split("/")
  return dest / segment / name


def is_synced(dest: Path, device_path: str, size: int) -> bool:
  path = local_path(dest, device_path)
  return path.is_file() and path.stat().st_size == size


def copy_batch(ssh_cmd, base: str, dest: Path, batch: dict[str, int]) -> tuple[int, int]:
  """Stream one tar of the batch over a single SSH connection. Returns (copied, bytes)."""
  proc = subprocess.Popen(ssh_cmd(f"cd {base} && tar -cf - -T -"),
                          stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  def feed():
    try:
      proc.stdin.write(("\n".join(batch) + "\n").encode())
      proc.stdin.close()
    except OSError:
      pass  # ssh died; its exit status reports it below

  stderr_chunks: list[bytes] = []
  threads = [threading.Thread(target=feed),
             threading.Thread(target=lambda: stderr_chunks.append(proc.stderr.read()))]
  for t in threads:
    t.start()

  copied = nbytes = 0
  try:
    with tarfile.open(fileobj=proc.stdout, mode="r|") as tar:
      for member in tar:
        # only ever write the exact paths we asked for, whatever the device sends back
        if member.name not in batch or not member.isreg():
          continue
        if member.size != batch[member.name]:
          print(f"  skipping {member.name}: it changed while copying (still being written?)", file=sys.stderr)
          continue
        target = local_path(dest, member.name)
        target.parent.mkdir(parents=True, exist_ok=True)
        tmp = target.with_name(target.name + ".part")
        with tar.extractfile(member) as src, open(tmp, "wb") as dst:
          shutil.copyfileobj(src, dst)
        tmp.replace(target)
        copied += 1
        nbytes += member.size
  except tarfile.TarError as e:
    print(f"  stream ended early: {e}", file=sys.stderr)
  finally:
    # read out the archive's trailing padding, or ssh dies on a closed pipe and looks like a lost connection
    while proc.stdout.read(1 << 20):
      pass
    proc.stdout.close()
    for t in threads:
      t.join()
    proc.wait()

  stderr = b"".join(stderr_chunks).decode(errors="replace").strip()
  if proc.returncode == 255:
    raise SystemExit(f"lost the SSH connection to the device:\n{stderr}")
  if stderr:
    print(f"  device said: {stderr}", file=sys.stderr)
  return copied, nbytes


def sync(args) -> int:
  def ssh_cmd(remote: str) -> list[str]:
    return ssh_command(args.ssh, args.host, args.port, args.identity, remote)

  dest = args.dest.expanduser()
  found, recording = list_device_rlogs(ssh_cmd, args.device_base, tuple(args.roots))
  device_files: dict[str, int] = {}
  for path, size in sorted(found.items()):
    # the same segment can sit in two of the device's folders; they save to one place
    if all(local_path(dest, path) != local_path(dest, other) for other in device_files):
      device_files[path] = size
  todo = {p: s for p, s in device_files.items() if not is_synced(dest, p, s)}
  have = len(device_files) - len(todo)

  print(f"device has {len(device_files)} rlogs; {have} already in {dest}, {len(todo)} to copy "
        f"({sum(todo.values()) / 1e6:.0f} MB)" + (f", {recording} still recording (skipped)" if recording else ""))
  if args.dry_run or not todo:
    if args.dry_run:
      for path, size in todo.items():
        print(f"  would copy {path}  {size / 1e6:.1f} MB")
    return 0

  dest.mkdir(parents=True, exist_ok=True)
  items = list(todo.items())
  copied = nbytes = 0
  for start in range(0, len(items), args.batch):
    batch = dict(items[start:start + args.batch])
    c, b = copy_batch(ssh_cmd, args.device_base, dest, batch)
    copied, nbytes = copied + c, nbytes + b
    print(f"  {min(start + args.batch, len(items))}/{len(items)} segments, {nbytes / 1e6:.0f} MB")

  missing = len(todo) - copied
  print(f"copied {copied} rlogs ({nbytes / 1e6:.0f} MB) to {dest}" + (f"; {missing} didn't make it, rerun to retry" if missing else ""))
  local = sum(1 for p in dest.glob("*/rlog*") if p.name in RLOG_NAMES)
  print(f"{dest} now holds {local} segments (~{local} min of driving, steering or not)")
  return 1 if missing else 0


def main(argv=None):
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("host", help="the device's IP or hostname, optionally user@host (default user: comma)")
  parser.add_argument("dest", type=Path, help="folder to copy into; point train_nnff.py at the same folder")
  parser.add_argument("--dry-run", action="store_true", help="only list what would be copied")
  parser.add_argument("--port", type=int, help="SSH port (default 22)")
  parser.add_argument("-i", "--identity", help="SSH private key file, if it isn't one ssh finds on its own")
  parser.add_argument("--batch", type=int, default=100, help="segments per SSH connection (default 100)")
  parser.add_argument("--roots", nargs="+", default=list(DEVICE_ROOTS), help=f"device folders under {DEVICE_BASE} (default: {' '.join(DEVICE_ROOTS)})")
  parser.add_argument("--device-base", default=DEVICE_BASE, help=argparse.SUPPRESS)
  parser.add_argument("--ssh", default="ssh", help="ssh executable (default: ssh)")
  args = parser.parse_args(argv)

  if "@" not in args.host:
    args.host = f"comma@{args.host}"
  if args.batch < 1:
    parser.error("--batch must be at least 1")
  bad = [r for r in args.roots if not ROOT_RE.match(r)]
  if bad:
    parser.error(f"--roots must be plain folder names, got {bad}")
  if shutil.which(args.ssh) is None:
    raise SystemExit(f"can't find '{args.ssh}'. On Windows, add the 'OpenSSH Client' optional feature (Settings > Optional features).")
  raise SystemExit(sync(args))


if __name__ == "__main__":
  main()
