from pathlib import Path

import pytest

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
