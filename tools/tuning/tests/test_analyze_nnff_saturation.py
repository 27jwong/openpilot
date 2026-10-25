from types import SimpleNamespace

import numpy as np
import pytest

from openpilot.tools.tuning import analyze_nnff_saturation as A

SIGLIN = (6.69417, 0.71168, 0.21504, 0.02331)


def fake_log(monkeypatch, events):
  monkeypatch.setattr(A.T, "load_log_schema", lambda cereal_dir=None: SimpleNamespace(
    Event=SimpleNamespace(read_multiple_bytes=lambda data: iter(events))))
  monkeypatch.setattr(A.T, "read_log_bytes", lambda path: b"")


def torque_event(t, desired, actual, output, version=0, saturated=False):
  state = SimpleNamespace(desiredLateralAccel=desired, actualLateralAccel=actual, f=0.5, p=0.1, i=0.2, output=output,
                          saturated=saturated, active=True, version=version)
  lcs = SimpleNamespace(which=lambda: "torqueState", torqueState=state)
  return SimpleNamespace(which=lambda: "controlsState", logMonoTime=int(t * 1e9), controlsState=SimpleNamespace(lateralControlState=lcs))


def car_state_event(t, v=15.0, pressed=False):
  return SimpleNamespace(which=lambda: "carState", logMonoTime=int(t * 1e9), carState=SimpleNamespace(vEgo=v, steeringPressed=pressed))


def alert_event(t, name="steerSaturated"):
  return SimpleNamespace(which=lambda: "onroadEvents", logMonoTime=int(t * 1e9), onroadEvents=[SimpleNamespace(name=name)])


def steady(seconds, desired, actual, output, version=0, t0=100.0, alert_at=None):
  """A route that holds one turn for a while, as the arrays assemble() hands to analyze()."""
  t = np.arange(t0, t0 + seconds, 0.01)
  n = len(t)
  return dict(t=t, desired=np.full(n, desired), actual=np.full(n, actual), f=np.zeros(n), p=np.zeros(n), i=np.zeros(n),
              output=np.full(n, -output), saturated=np.zeros(n), active=np.ones(n), version=np.full(n, float(version)),
              v_ego=np.full(n, 15.0), pressed=np.zeros(n), alerts=np.array([] if alert_at is None else [alert_at]))


def test_a_turn_the_car_undershoots_at_full_torque_is_what_the_alert_fires_on():
  a = A.analyze(steady(10, desired=2.0, actual=1.4, output=1.0, alert_at=105.0), SIGLIN)
  assert a["controller"] == "NNFF"
  assert a["would_alert"] == pytest.approx(10.0, abs=0.05)
  b = next(b for b in a["bins"] if b["lo"] == 2.0)
  assert (b["undershoot"], b["at_max"], b["output"], b["track"]) == pytest.approx((1.0, 1.0, 1.0, 0.7))
  assert b["siglin"] == pytest.approx(abs(A.T.siglin(2.0, *SIGLIN)))
  assert a["out_gap"] > 0  # commanded more than the static curve says
  assert a["alerts"][0]["desired"] == pytest.approx(2.0) and a["alerts"][0]["output"] == pytest.approx(1.0)


def test_a_turn_the_car_follows_does_not_count():
  a = A.analyze(steady(10, desired=2.0, actual=1.9, output=0.8), SIGLIN)
  assert a["would_alert"] == 0.0
  assert next(b for b in a["bins"] if b["lo"] == 2.0)["undershoot"] == 0.0


def test_driver_steering_and_standstill_are_left_out():
  d = steady(10, desired=2.0, actual=1.0, output=1.0)
  d["pressed"][:] = 1.0
  assert A.analyze(d, SIGLIN)["engaged"] == 0.0


def test_the_logged_version_says_which_controller_steered():
  assert A.controller_name(0.0) == "NNFF"
  assert A.controller_name(2.0) == "torque"
  assert A.analyze(steady(5, 1.2, 1.2, 0.5, version=2), SIGLIN)["controller"] == "torque"


def init_event(t, wall_ns):
  return SimpleNamespace(which=lambda: "initData", logMonoTime=int(t * 1e9), initData=SimpleNamespace(wallTimeNanos=wall_ns))


def goat_event(t):
  return SimpleNamespace(which=lambda: "starpilotOnroadEvents", logMonoTime=int(t * 1e9),
                         starpilotOnroadEvents=SimpleNamespace(events=[SimpleNamespace(name="goatSteerSaturated")]))


def test_extract_reads_the_state_the_logged_alerts_and_when_the_route_started(monkeypatch):
  fake_log(monkeypatch, [init_event(5.0, 1_700_000_000_000_000_000), car_state_event(10.0), torque_event(10.0, 2.0, 1.4, -1.0), alert_event(10.5),
                         alert_event(10.6, "pcmEnable"), goat_event(10.7), car_state_event(11.0), torque_event(11.0, 2.0, 1.5, -1.0)])
  seg = A.extract_segment("rlog.zst")
  assert list(seg["desired"]) == [2.0, 2.0]
  assert seg["alerts"] == pytest.approx([10.5, 10.7])  # the stock alert and the goat-scream one, nothing else
  assert list(seg["start"]) == [1.7e9, 5.0]
  d = A.assemble([seg])
  assert d["v_ego"] == pytest.approx([15.0, 15.0])
  assert list(d["start"]) == [1.7e9, 5.0]


def old_alert_event(t):
  return SimpleNamespace(which=lambda: "onroadEventsDEPRECATED", logMonoTime=int(t * 1e9), onroadEventsDEPRECATED=[SimpleNamespace(name="steerSaturated")])


def test_alerts_in_logs_from_before_the_carEvents_rename_are_counted(monkeypatch):
  fake_log(monkeypatch, [car_state_event(10.0), torque_event(10.0, 2.0, 1.4, -1.0), old_alert_event(10.5)])
  assert A.extract_segment("rlog.zst")["alerts"] == pytest.approx([10.5])


def test_a_route_is_dated_by_the_clock_after_it_synced(monkeypatch):
  # the first two segments were written on the default clock (a date in the past), the third after the sync
  segs = []
  for mono, wall in ((5.0, 1.0e9), (65.0, 1.0e9 + 60), (125.0, 1.7e9 + 120)):
    fake_log(monkeypatch, [init_event(mono, int(wall * 1e9)), car_state_event(mono + 1), torque_event(mono + 1, 2.0, 1.4, -1.0),
                           car_state_event(mono + 2), torque_event(mono + 2, 2.0, 1.4, -1.0)])
    segs.append(A.extract_segment("rlog.zst"))
  assert list(A.assemble(segs)["start"]) == pytest.approx([1.7e9, 5.0])  # the synced clock, at the first segment's start


def test_stretches_of_trouble_are_grouped_and_dated():
  calm = steady(3, desired=0.5, actual=0.5, output=0.3, t0=110.0)
  first, second = steady(2, 2.0, 1.4, 1.0, t0=100.0), steady(2, 2.5, 1.5, 1.0, t0=120.0)
  d = {k: np.concatenate([first[k], calm[k], second[k]]) for k in first if k != "alerts"}
  d["alerts"] = np.array([])
  d["start"] = np.array([1.7e9, 100.0])
  episodes = A.analyze(d, SIGLIN)["episodes"]
  assert len(episodes) == 2
  assert [e["seconds"] for e in episodes] == pytest.approx([2.0, 2.0], abs=0.02)
  assert episodes[0]["wall"] == pytest.approx(1.7e9)  # the stretch began at the route's start
  assert episodes[1]["wall"] == pytest.approx(1.7e9 + 20.0)
  assert episodes[1]["deficit"] == pytest.approx(1.0)  # 2.5 wanted, 1.5 got


def test_logs_from_another_schema_raise_instead_of_looking_truncated(monkeypatch):
  class Bad:
    @property
    def torqueState(self):
      raise AttributeError("struct has no such member: version")

    def which(self):
      return "torqueState"

  fake_log(monkeypatch, [SimpleNamespace(which=lambda: "controlsState", logMonoTime=0, controlsState=SimpleNamespace(lateralControlState=Bad()))])
  with pytest.raises(AttributeError, match="version"):
    A.extract_segment("rlog.zst")


def test_route_counters_order_drives_and_tolerate_old_names():
  assert A.route_counter("000002d1--068db1d740") > A.route_counter("000002a0--abcdef0123")
  assert A.route_counter("2024-03-02--19-02-46") == -1


def write_turn_rlog(path):
  """Two seconds of an NNFF turn the car can't keep up with, alerting the old way, as one rlog segment."""
  from cereal import log
  import zstandard
  msgs = []
  for k in range(200):
    t = int((100 + k * 0.01) * 1e9)
    cs = log.Event.new_message(logMonoTime=t, valid=True)
    cs.init("carState").vEgo = 15.0
    ctl = log.Event.new_message(logMonoTime=t, valid=True)
    s = ctl.init("controlsState").init("lateralControlState").init("torqueState")
    s.active, s.desiredLateralAccel, s.actualLateralAccel, s.output = True, 2.0, 1.4, -1.0
    msgs += [cs.to_bytes(), ctl.to_bytes()]
  alert = log.Event.new_message(logMonoTime=int(101e9), valid=True)
  alert.init("onroadEventsDEPRECATED", 1)[0].name = "steerSaturated"  # how logs from before the carEvents rename hold it
  msgs.append(alert.to_bytes())
  path.parent.mkdir(parents=True)
  path.write_bytes(zstandard.ZstdCompressor().compress(b"".join(msgs)))


def test_logs_are_read_with_a_resolved_copy_of_cereal_when_git_left_symlink_stubs(tmp_path):
  # capnp aborts the worker on a stub rather than raising (BrokenProcessPool), so the workers must be handed the copy
  import os
  import subprocess
  import sys
  from openpilot.tools.tuning.tests.test_plot_lateral_delay import TOOLS, stubbed_cereal
  cereal = stubbed_cereal(tmp_path)
  write_turn_rlog(tmp_path / "logs" / "000002d1--068db1d740--0" / "rlog.zst")
  home = {"HOME": str(tmp_path / "home"), "USERPROFILE": str(tmp_path / "home")}  # where the resolved copy is cached

  run = subprocess.run([sys.executable, str(TOOLS / "analyze_nnff_saturation.py"), str(tmp_path / "logs"), "--cereal", str(cereal),
                        "--workers", "1"], capture_output=True, text=True, timeout=300, env={**os.environ, **home})

  assert run.returncode == 0, run.stdout + run.stderr
  assert "000002d1--068db1d740" in run.stdout and "NNFF" in run.stdout
  assert "1 logged alerts" in run.stdout
  assert (cereal / "car.capnp").read_text() == "../opendbc_repo/car.capnp"  # the checkout is left alone
