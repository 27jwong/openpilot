from pathlib import Path

import numpy as np
import pytest

from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.locationd import longlagd
from openpilot.selfdrive.locationd.test.test_longlagd import DT, make_command, simulate
from openpilot.starpilot.common.longitudinal_delay import brake_threshold
from openpilot.tools.tuning import analyze_longitudinal as A
from openpilot.tools.tuning.analyze_longitudinal import resolve_route

ROUTE = "00000005--58d6714619"


def make_realdata(root: Path) -> Path:
  realdata = root / "realdata"
  for name, segments in ((ROUTE, (2, 0, 1)), ("00000006--0123456789", (0,)), ("2026-10-01--12-34-56", (0, 1))):
    for segment in segments:
      folder = realdata / f"{name}--{segment}"
      folder.mkdir(parents=True)
      (folder / "rlog.zst").write_bytes(b"")
      (folder / "qlog.zst").write_bytes(b"")
  return realdata


def segment_names(paths):
  return [Path(p).parent.name for p in paths]


def test_route_name_gives_every_segment_in_order(tmp_path):
  realdata = make_realdata(tmp_path)
  assert segment_names(resolve_route(ROUTE, [realdata])) == [f"{ROUTE}--0", f"{ROUTE}--1", f"{ROUTE}--2"]
  assert all(Path(p).name == "rlog.zst" for p in resolve_route(ROUTE, [realdata]))
  assert segment_names(resolve_route("2026-10-01--12-34-56", [realdata])) == ["2026-10-01--12-34-56--0", "2026-10-01--12-34-56--1"]


def test_all_digit_route_is_not_read_as_a_segment(tmp_path):
  realdata = make_realdata(tmp_path)
  assert segment_names(resolve_route("00000006--0123456789", [realdata])) == ["00000006--0123456789--0"]


def test_segment_name_gives_that_segment(tmp_path, capsys):
  realdata = make_realdata(tmp_path)
  assert segment_names(resolve_route(f"{ROUTE}--1", [realdata])) == [f"{ROUTE}--1"]
  assert ROUTE in capsys.readouterr().out  # points at the whole drive


def test_directories_and_extra_log_dirs(tmp_path):
  realdata = make_realdata(tmp_path)
  assert segment_names(resolve_route(str(realdata / f"{ROUTE}--2"), [])) == [f"{ROUTE}--2"]
  # a missing default folder (a PC has no /data/media/0/realdata) is skipped, not an error
  assert len(resolve_route(ROUTE, [tmp_path / "missing", realdata])) == 3
  with pytest.raises(SystemExit, match="3 routes"):
    resolve_route(str(realdata), [])


@pytest.mark.parametrize("identifier", ["a2a0ccea32023010/00000005--58d6714619", "a2a0ccea32023010|00000005--58d6714619--4",
                                        "https://connect.comma.ai/a2a0ccea32023010/00000005--58d6714619"])
def test_connect_ids_pass_through(tmp_path, identifier):
  assert resolve_route(identifier, [make_realdata(tmp_path)]) == [identifier]


def test_file_passes_through(tmp_path):
  rlog = make_realdata(tmp_path) / f"{ROUTE}--0" / "rlog.zst"
  assert resolve_route(str(rlog), []) == [str(rlog)]


def test_unknown_route_says_where_it_looked(tmp_path):
  realdata = make_realdata(tmp_path)
  with pytest.raises(SystemExit, match="realdata"):
    resolve_route("00000099--ffffffffff", [realdata])
  with pytest.raises(SystemExit, match="No rlogs"):
    resolve_route(f"{ROUTE}--9", [realdata])


# *** pooling drives, and only claiming what the data supports ***


def engaged_drive(seconds, t0=0.0, **overrides):
  n = int(seconds / DT_CTRL)
  d = {key: np.zeros(n) for key in ("v_ego", "a_ego", "a_target", "accel_cmd", "v_ego_raw", "brake_pressure",
                                    "gas_pressed", "brake_pressed", "has_lead")}
  d.update(t=t0 + np.arange(n) * DT_CTRL, long_active=np.ones(n), blend_ok=np.ones(n),
           long_control_state=np.full(n, float(A.LongCtrlStatePid)), v_ego=np.full(n, 20.0), v_ego_raw=np.full(n, 20.0))
  d.update(overrides)
  return d


def delay_series(cmd, v, okay=None):
  t = np.arange(len(cmd)) * DT
  accel, usable = longlagd.true_accel(t, v, np.ones(len(t), dtype=bool) if okay is None else okay)
  return {"t": t, "v": v, "cmd": cmd, "a_ego": accel, "accel": accel, "usable": usable,
          "thr": np.array([brake_threshold(x) for x in v]), "pressure": np.full(len(t), np.nan)}


def test_pooled_drives_never_share_a_run():
  # two drives logged by different boots: their clocks overlap
  d = A._concat_routes([engaged_drive(60, t0=1000.0), engaged_drive(90, t0=5.0)])
  assert np.all(np.diff(d["t"]) > 0)
  runs = A._engaged_runs(d)
  assert [round((b - a) * DT_CTRL) for a, b in runs] == [60, 90]


def test_coverage_says_where_the_minutes_went():
  n = int(100 / DT_CTRL)
  blend_ok, gas = np.ones(n), np.zeros(n)
  blend_ok[:n // 2] = 0.0          # first half handed to the stock ACC
  gas[n // 2:n // 2 + 1000] = 1.0  # ten seconds of driver gas
  c = A.coverage(engaged_drive(100, blend_ok=blend_ok, gas_pressed=gas))
  assert round(c["blended"]) == 50 and round(c["pedals"]) == 10
  assert round(c["usable"]) == 40 and round(c["logs"]) == 100


@pytest.mark.parametrize("r2s, kind", [
  ([0.60, 0.601, 0.605, 0.602], "flat"),
  ([0.63, 0.62, 0.61, 0.60], "single"),
  ([0.60, 0.61, 0.62, 0.63], "edge"),
  ([0.90, 0.95, 0.99, 0.94], "interior"),
  ([None, None, 0.9], "no data"),
])
def test_threshold_sweep_reading(r2s, kind):
  assert A._classify_sweep(r2s)[0] == kind


def test_verdict_finds_a_real_gas_brake_gap():
  t, cmd = make_command(900, seed=2)
  verdict = A.gas_brake_verdict(delay_series(cmd, simulate(cmd, 0.25, 0.5)), "interior", 0.0)
  assert verdict.startswith("The brakes answer") and "later" in verdict


def test_verdict_calls_equal_delays_alike():
  t, cmd = make_command(900, seed=2)
  verdict = A.gas_brake_verdict(delay_series(cmd, simulate(cmd, 0.35, 0.35)), "flat", None)
  assert verdict.startswith("Gas and brake answer alike")


def test_verdict_declines_on_thin_or_noisy_data():
  t, cmd = make_command(150, seed=2)
  assert A.gas_brake_verdict(delay_series(cmd, simulate(cmd, 0.25, 0.5)), "interior", 0.0).startswith("Only 3 minute-long")

  # like a real drive: mostly gas, noisy wheel speed, and the engaged stretches broken up
  t, cmd = make_command(900, seed=4, offset=0.5, scale=0.7)
  v = simulate(cmd, 0.25, 0.5, noise=0.02, seed=5)
  okay = (np.arange(len(t)) * DT) % 30 < 20
  verdict = A.gas_brake_verdict(delay_series(cmd, v, okay), "single", None)
  assert verdict.startswith("Can't tell"), verdict
