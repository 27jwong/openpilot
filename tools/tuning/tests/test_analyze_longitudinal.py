from array import array
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


# *** keeping memory proportional to the usable minutes ***


def test_logged_rows_resample_by_field():
  # three samples 10ms apart, every field a different number so a column mix-up shows
  rows = array("d")
  for i in range(3):
    rows.extend([0.01 * i] + [10.0 * (k + 1) + i for k in range(len(A.FIELDS) - 1)])
  d = A._resample(rows)
  assert set(d) == set(A.FIELDS) and d["t"][0] == 0.0
  assert d["v_ego"][0] == 10.0 and d["brake_pressure"][0] == 60.0       # interpolated
  assert d["long_active"][0] == 70.0 and d["blend_ok"][0] == 120.0      # held


def blended_pattern(seconds, on=40.0, off=15.0):
  """blend_ok for a drive that is engaged for `on` seconds, then handed to the stock ACC for `off`."""
  s = np.arange(int(seconds / DT_CTRL)) * DT_CTRL
  return (s % (on + off) < on).astype(float)


def two_path_drive(seconds, gas=0.25, brake=0.5, seed=2):
  """A 100Hz drive whose wheel speed answers longlagd's two-path model, engaged in stretches."""
  t, cmd = make_command(seconds, seed=seed)
  v = simulate(cmd, gas, brake)
  grid = np.arange(int(seconds / DT_CTRL)) * DT_CTRL
  v, cmd = np.interp(grid, t, v), np.interp(grid, t, cmd)
  accel = np.gradient(v, DT_CTRL)
  return engaged_drive(seconds, v_ego=v, v_ego_raw=v, accel_cmd=cmd, a_ego=accel, a_target=cmd,
                       blend_ok=blended_pattern(seconds))


def test_engaged_spans_keep_the_runs_and_drop_the_rest():
  d = two_path_drive(600)
  runs, spans = A._engaged_runs(d), A.engaged_spans(d)
  assert len(spans) == len(runs) == 11
  kept = sum(len(s["t"]) for s in spans)
  assert 0.7 * len(d["t"]) < kept < 0.9 * len(d["t"])   # 40 of every 55 seconds, plus the pads
  pooled = A._concat_routes(spans)
  assert [round((b - a) * DT_CTRL) for a, b in A._engaged_runs(pooled)] == [round((b - a) * DT_CTRL) for a, b in runs]


def test_engaged_spans_merge_pads_that_overlap():
  blend_ok = np.ones(int(40 / DT_CTRL))
  for a, b in ((10, 14), (20, 28), (35, 40)):   # runs: 0-10, 14-20, 28-35
    blend_ok[int(a / DT_CTRL):int(b / DT_CTRL)] = 0.0
  spans = A.engaged_spans(engaged_drive(40, blend_ok=blend_ok))
  # a 4s break is inside both pads, so the first two runs share a span; the 8s break is not
  assert [round(len(s["t"]) * DT_CTRL, 1) for s in spans] == [23.5, 14.0]


def test_cut_drives_give_the_same_answers(capsys):
  d = two_path_drive(900)
  whole = A._concat_routes([d])
  cut = A._concat_routes(A.engaged_spans(d))
  assert len(cut["t"]) < len(whole["t"])

  A.summarize_static_map(A._engaged_runs(whole), whole)
  A.summarize_plant(A._engaged_runs(whole), whole)
  full_text = capsys.readouterr().out
  A.summarize_static_map(A._engaged_runs(cut), cut)
  A.summarize_plant(A._engaged_runs(cut), cut)
  assert capsys.readouterr().out == full_text

  # the 20Hz series starts on a different 100Hz sample, so the fits agree to within noise rather than exactly
  fits = []
  for x in (whole, cut):
    s = A._delay_series(A._engaged_runs(x), x)
    fits.append(longlagd.fit_split_delay(s["cmd"], s["accel"], s["thr"], s["usable"], A.DELAY_DT))
  assert fits[0].r2 == pytest.approx(fits[1].r2, abs=0.02)
  for name in ("gas", "brake"):
    assert fits[0].regimes[name].delay == pytest.approx(fits[1].regimes[name].delay, abs=0.06)


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
