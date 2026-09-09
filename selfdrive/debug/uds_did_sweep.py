#!/usr/bin/env python3
"""
Sweep UDS ReadDataByIdentifier (0x22) on an ECU and report every DID that answers.

Built to answer one question on GEN2 Mazdas: does the forward radar (0x764) expose
object/target data over diagnostics? Its tracks are not on CAN -- the only radar-derived
signal on the bus is a single lead-detected bit in CRUZE_STATE (0x44A). If the radar
does not answer here either, its perception is sealed and the only remaining path is
an interceptor on the radar's own harness.

Two modes:

  sweep   walk a DID range, print/record every positive response  (default)
  watch   poll one DID repeatedly, report which bytes move and how fast it polls

Read-only: it sends 0x22 (ReadDataByIdentifier), 0x10 (session control) and 0x3E
(tester present), and nothing else. No writes, no routine control, no security access.

Run it on the comma device, parked, with openpilot stopped. See --help.
"""
import argparse
import json
import os
import struct
import sys
import time
from subprocess import CalledProcessError, check_output

from opendbc.car.carlog import carlog
from opendbc.car.structs import CarParams
from opendbc.car.uds import (SESSION_TYPE, InvalidServiceIdError, InvalidSubFunctionError,
                             MessageTimeoutError, NegativeResponseError, UdsClient)
from panda import Panda

# DID ranges, most-likely-first. A full 16-bit sweep is ~65k requests; at the default
# timeout that is roughly 15-30 min, and --out makes it resumable.
STAGED_RANGES = [
  (0xF180, 0xF1FF, "standard identification (sanity check -- expect hits here)"),
  (0x2000, 0x2FFF, "common OEM live-data block"),
  (0x0100, 0x03FF, "low OEM block"),
  (0xD000, 0xDFFF, "high OEM block"),
  (0x0000, 0x00FF, "remainder"),
  (0x0400, 0x1FFF, "remainder"),
  (0x3000, 0xCFFF, "remainder"),
  (0xE000, 0xF17F, "remainder"),
  (0xF200, 0xFFFF, "remainder"),
]

# Negative response codes that mean "this DID isn't here", as opposed to something
# we should tell the user about.
BORING_NRCS = {
  0x11,  # serviceNotSupported
  0x12,  # subFunctionNotSupported
  0x31,  # requestOutOfRange  <- the overwhelmingly common "no such DID"
}


def parked_confirmation(skip: bool) -> None:
  if skip:
    return
  print(__doc__.strip())
  print()
  print("!! This talks to a safety-critical ECU and puts it in a diagnostic session.")
  print("!! The car must be PARKED. Do not drive while this runs.")
  print("!! ACC/FCW may stay disabled until you cycle the ignition.")
  print()
  if input("Type 'parked' to continue: ").strip().lower() != "parked":
    print("aborted")
    sys.exit(1)


def check_pandad_stopped() -> None:
  try:
    check_output(["pidof", "pandad"])
  except CalledProcessError as e:
    if e.returncode == 1:
      return  # not running, which is what we want
    raise
  print("pandad is running -- stop openpilot first (aborted)")
  sys.exit(1)


def make_client(args) -> UdsClient:
  panda = Panda()
  panda.set_safety_mode(CarParams.SafetyModel.elm327)
  return UdsClient(panda, args.addr, bus=args.bus, timeout=args.timeout,
                   response_pending_timeout=args.pending_timeout)


def enter_session(client: UdsClient, session: str) -> None:
  if session == "none":
    return
  st = SESSION_TYPE.EXTENDED_DIAGNOSTIC if session == "extended" else SESSION_TYPE.DEFAULT
  try:
    client.diagnostic_session_control(st)
    print(f"entered {st.name} session")
  except Exception as e:
    print(f"could not enter {st.name} session ({e}) -- continuing anyway")


def read_did(client: UdsClient, did: int):
  """Return (data, None) on success, (None, reason) on a normal miss.

  Re-raises nothing: a sweep should never die on one bad DID.
  """
  try:
    return client.read_data_by_identifier(did), None
  except NegativeResponseError as e:
    return None, ("nrc", e.error_code, str(e))
  except MessageTimeoutError:
    return None, ("timeout", None, "timeout")
  except (InvalidServiceIdError, InvalidSubFunctionError, ValueError) as e:
    # ValueError covers the echo-mismatch check in read_data_by_identifier, which
    # some ECUs trip by replying to a different DID than we asked for. Worth seeing.
    return None, ("malformed", None, str(e))
  except Exception as e:  # noqa: BLE001 - never let the sweep die
    return None, ("error", None, f"{type(e).__name__}: {e}")


def build_did_list(args) -> list[int]:
  if args.start is not None or args.end is not None:
    start = args.start if args.start is not None else 0x0000
    end = args.end if args.end is not None else 0xFFFF
    return list(range(start, end + 1))
  dids: list[int] = []
  for lo, hi, _desc in (STAGED_RANGES if args.full else STAGED_RANGES[:4]):
    dids.extend(range(lo, hi + 1))
  return dids


def do_sweep(client: UdsClient, args) -> None:
  results: dict[str, dict] = {}
  if args.out and os.path.exists(args.out):
    with open(args.out) as f:
      results = json.load(f)
    print(f"resuming: {len(results)} DIDs already probed in {args.out}")

  dids = [d for d in build_did_list(args) if f"0x{d:04x}" not in results]
  print(f"sweeping {len(dids)} DIDs on {hex(args.addr)} (bus {args.bus})\n")

  hits, t_start, last_keepalive, last_save = 0, time.monotonic(), time.monotonic(), time.monotonic()
  try:
    for i, did in enumerate(dids):
      if time.monotonic() - last_keepalive > 2.0:
        try:
          client.tester_present()
        except Exception:
          pass
        last_keepalive = time.monotonic()

      data, miss = read_did(client, did)
      key = f"0x{did:04x}"

      if data is not None:
        hits += 1
        results[key] = {"ok": True, "len": len(data), "hex": data.hex()}
        print(f"  HIT {key}  len={len(data):3}  {data.hex()}")
      else:
        kind, nrc, msg = miss
        results[key] = {"ok": False, "why": kind, "nrc": nrc}
        if kind != "nrc" or nrc not in BORING_NRCS:
          print(f"  ??  {key}  {msg}")

      if args.delay:
        time.sleep(args.delay)

      if i % 500 == 499:
        done, total = i + 1, len(dids)
        rate = done / (time.monotonic() - t_start)
        eta = (total - done) / rate if rate else 0
        print(f"  ... {done}/{total}  {hits} hits  {rate:.0f} DID/s  eta {eta/60:.1f} min")

      if args.out and time.monotonic() - last_save > 10:
        with open(args.out, "w") as f:
          json.dump(results, f, indent=1, sort_keys=True)
        last_save = time.monotonic()
  except KeyboardInterrupt:
    print("\ninterrupted")
  finally:
    if args.out:
      with open(args.out, "w") as f:
        json.dump(results, f, indent=1, sort_keys=True)
      print(f"\nwrote {args.out}")

  ok = {k: v for k, v in results.items() if v.get("ok")}
  print(f"\n=== {len(ok)} responding DIDs ===")
  for k in sorted(ok, key=lambda x: int(x, 16)):
    v = ok[k]
    print(f"  {k}  len={v['len']:3}  {v['hex']}")
  if ok:
    print("\nNext: pick the DIDs whose payload looks like it could hold targets and run")
    print(f"  {sys.argv[0]} watch --did 0x1234   (with a car in front of you)")
    print("A DID whose bytes never move is config; a DID that moves with traffic is perception.")


def do_watch(client: UdsClient, args) -> None:
  did = args.did
  print(f"polling DID {hex(did)} on {hex(args.addr)} for {args.seconds}s -- "
        f"point the car at moving traffic\n")
  samples: list[bytes] = []
  stamps: list[float] = []
  t_end = time.monotonic() + args.seconds
  last_keepalive = time.monotonic()
  try:
    while time.monotonic() < t_end:
      if time.monotonic() - last_keepalive > 2.0:
        try:
          client.tester_present()
        except Exception:
          pass
        last_keepalive = time.monotonic()
      t0 = time.monotonic()
      data, miss = read_did(client, did)
      if data is None:
        print(f"  miss: {miss[2]}")
        time.sleep(0.1)
        continue
      samples.append(data)
      stamps.append(t0)
      if args.print_every and len(samples) % args.print_every == 0:
        print(f"  {time.monotonic()-t0:6.3f}s  {data.hex()}")
  except KeyboardInterrupt:
    print("\ninterrupted")

  if len(samples) < 2:
    print("not enough samples")
    return

  n = min(len(s) for s in samples)
  if any(len(s) != n for s in samples):
    print(f"warning: response length varies ({sorted({len(s) for s in samples})}); "
          f"comparing first {n} bytes")

  dt = [b - a for a, b in zip(stamps, stamps[1:], strict=False)]
  hz = len(samples) / (stamps[-1] - stamps[0])
  print(f"\n{len(samples)} samples, {hz:.1f} Hz "
        f"(median period {sorted(dt)[len(dt)//2]*1000:.1f} ms), {len(set(samples))} unique")
  print("\nper-byte activity:")
  print(f"  {'byte':>4} {'uniq':>5} {'min':>4} {'max':>4}  changes")
  for i in range(n):
    col = [s[i] for s in samples]
    changes = sum(1 for a, b in zip(col, col[1:], strict=False) if a != b)
    mark = "  <-- moving" if len(set(col)) > 4 else ""
    print(f"  {i:>4} {len(set(col)):>5} {min(col):>4} {max(col):>4}  {changes:>6}{mark}")
  print("\nIf several adjacent bytes move together and settle when traffic clears,")
  print("that is a target list. Log it alongside a route and correlate against")
  print("modelV2.leadsV3 the same way the CAN sweep did.")


def main() -> None:
  p = argparse.ArgumentParser(description="UDS ReadDataByIdentifier sweep / watch",
                              formatter_class=argparse.RawDescriptionHelpFormatter,
                              epilog="examples:\n"
                                     "  ./uds_did_sweep.py sweep\n"
                                     "  ./uds_did_sweep.py sweep --full --out radar_dids.json\n"
                                     "  ./uds_did_sweep.py watch --did 0x2100 --seconds 60\n")
  p.add_argument("mode", choices=["sweep", "watch"], nargs="?", default="sweep")
  p.add_argument("--addr", type=lambda x: int(x, 0), default=0x764,
                 help="ECU tx address (default 0x764, GEN2 Mazda fwdRadar)")
  p.add_argument("--bus", type=int, default=0, help="CAN bus (default 0)")
  p.add_argument("--session", choices=["extended", "default", "none"], default="extended",
                 help="diagnostic session to enter first (default extended)")
  p.add_argument("--timeout", type=float, default=0.15,
                 help="per-request timeout in s (default 0.15; a local ECU answers in ms)")
  p.add_argument("--pending-timeout", type=float, default=1.0,
                 help="timeout after a 0x78 response-pending (default 1.0)")
  p.add_argument("--delay", type=float, default=0.0,
                 help="sleep between requests, if the ECU gets grumpy (default 0)")
  p.add_argument("--start", type=lambda x: int(x, 0), help="explicit DID range start")
  p.add_argument("--end", type=lambda x: int(x, 0), help="explicit DID range end")
  p.add_argument("--full", action="store_true", help="sweep all 65536 DIDs, not just likely blocks")
  p.add_argument("--out", help="JSON results file (makes the sweep resumable)")
  p.add_argument("--did", type=lambda x: int(x, 0), help="watch mode: DID to poll")
  p.add_argument("--seconds", type=float, default=30.0, help="watch mode: duration (default 30)")
  p.add_argument("--print-every", type=int, default=10,
                 help="watch mode: print every Nth sample (0 to silence)")
  p.add_argument("--yes", action="store_true", help="skip the parked confirmation")
  p.add_argument("--debug", action="store_true")
  args = p.parse_args()

  if args.mode == "watch" and args.did is None:
    p.error("watch mode needs --did")
  if args.debug:
    carlog.setLevel("DEBUG")

  parked_confirmation(args.yes)
  check_pandad_stopped()

  client = make_client(args)
  enter_session(client, args.session)

  if args.mode == "sweep":
    do_sweep(client, args)
  else:
    do_watch(client, args)


if __name__ == "__main__":
  main()
