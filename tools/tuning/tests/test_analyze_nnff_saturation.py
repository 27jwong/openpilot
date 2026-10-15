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


def test_extract_reads_the_state_and_the_logged_alerts(monkeypatch):
  fake_log(monkeypatch, [car_state_event(10.0), torque_event(10.0, 2.0, 1.4, -1.0), alert_event(10.5), alert_event(10.6, "pcmEnable"),
                         car_state_event(11.0), torque_event(11.0, 2.0, 1.5, -1.0)])
  seg = A.extract_segment("rlog.zst")
  assert list(seg["desired"]) == [2.0, 2.0]
  assert list(seg["alerts"]) == [10.5]  # only steerSaturated
  d = A.assemble([seg])
  assert d["v_ego"] == pytest.approx([15.0, 15.0])


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
