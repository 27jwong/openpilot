import json
from types import SimpleNamespace

import numpy as np
import pytest
import zstandard

from cereal import log
from openpilot.starpilot.controls.lib.neural_network_feedforward import NNFF_MODELS_PATH, FluxModel
from openpilot.tools.tuning import train_nnff as T

# The CX-30's current siglin, used as the synthetic car's true steering curve
PLANT = (6.69417, 0.71168, 0.21504, 0.02331)
LAG = 0.3
ROLL = 0.02


def plant_torque(lat_accel, roll):
  return T.siglin(lat_accel - T.GRAVITY * roll, *PLANT)


def lat_accel_at(t, phase):
  return 1.6 * np.sin(2 * np.pi * t / 9.0 + phase) + 0.6 * np.sin(2 * np.pi * t / 2.7 + 2 * phase)


def v_ego_at(t):
  return 20.0 + 4.0 * np.sin(2 * np.pi * t / 30.0)


def write_segment(path, t0, seconds, phase, disengaged=None, fingerprint="MAZDA_CX_30", lag_at=lambda t: LAG):
  """One synthetic rlog.zst where openpilot steers a car whose torque -> lat accel map is PLANT, lag_at(t) late."""
  events = []

  def event(t, which):
    evt = log.Event.new_message(logMonoTime=int(t * 1e9), valid=True)
    return evt, evt.init(which)

  for t in np.arange(t0, t0 + seconds, T.DT):
    v, la = v_ego_at(t), lat_accel_at(t, phase)
    evt, cs = event(t, "carState")
    cs.vEgo = float(v)
    events.append(evt)
    evt, cc = event(t + 0.001, "carControl")
    cc.latActive = not (disengaged is not None and disengaged[0] <= t < disengaged[1])
    events.append(evt)
    evt, co = event(t + 0.002, "carOutput")
    # LatControlNNFF sends -output_torque, so the logged torque is the plant's torque negated
    co.actuatorsOutput.torque = float(-plant_torque(lat_accel_at(t + lag_at(t), phase), ROLL))
    events.append(evt)
    evt, ctl = event(t + 0.003, "controlsState")
    ctl.curvature = float(la / v ** 2)
    events.append(evt)
  for t in np.arange(t0, t0 + seconds, 0.05):
    evt, lp = event(t + 0.004, "liveParameters")
    lp.roll = ROLL
    events.append(evt)
  for t in np.arange(t0, t0 + seconds, 0.25):  # lagd publishes at 4 Hz
    evt, ld = event(t + 0.005, "liveDelay")
    ld.lateralDelay = lag_at(t)
    events.append(evt)
  evt, cp = event(t0 + 0.6, "carParams")
  cp.carFingerprint = fingerprint
  cp.steerActuatorDelay = 0.1
  events.append(evt)

  events.sort(key=lambda e: e.logMonoTime)
  path.parent.mkdir(parents=True, exist_ok=True)
  path.write_bytes(zstandard.ZstdCompressor().compress(b"".join(e.to_bytes() for e in events)))


@pytest.fixture(scope="module")
def log_dir(tmp_path_factory):
  root = tmp_path_factory.mktemp("realdata")
  for r in range(3):
    for seg in range(2):
      write_segment(root / f"0000000{r}--abcdef{r}--{seg}" / "rlog.zst", 1000.0 * r + 40.0 * seg, 40.0, phase=r)
  return root


def test_inputs_match_what_lat_control_nnff_feeds_the_model():
  with open(NNFF_MODELS_PATH / "MAZDA_CX9_2021.json") as f:
    assert T.INPUT_VARS == json.load(f)["input_vars"]
  src = (T.REPO_ROOT / "starpilot/controls/lib/neural_network_feedforward.py").read_text()
  assert f"self.past_times = {T.PAST_TIMES}" in src
  assert f"self.future_times = {T.FUTURE_TIMES}" in src


def test_finds_segments_by_route(log_dir):
  routes = T.find_segments([log_dir])
  assert list(routes) == ["00000000--abcdef0", "00000001--abcdef1", "00000002--abcdef2"]
  assert [seg for seg, _ in routes["00000000--abcdef0"]] == [0, 1]


def test_rows_line_torque_up_with_the_lat_accel_it_caused(log_dir):
  segs = T.find_segments([log_dir])["00000000--abcdef0"]
  route = T.assemble_route("r", [T.extract_segment(path) for _, path in segs])
  assert route.fingerprint == "MAZDA_CX_30"
  assert route.lag == pytest.approx(LAG)

  X, y = T.build_rows(route)
  assert len(X) > 1000
  # the target is torque in LatControlNNFF's convention, and lat accel input is the one LAG later
  assert y == pytest.approx(plant_torque(X[:, 1], X[:, 3]), abs=5e-3)
  # history columns sit where LatControlNNFF samples them
  i = T.row_indices(route)
  c = i + int(round(LAG / T.DT))
  assert X[:, 4:11] == pytest.approx(np.column_stack([route.lat_accel[c + k] for k in T.HISTORY_FRAMES]))
  assert X[:, 11:] == pytest.approx(ROLL)
  # speed is the current one, not lagged like the rest
  assert X[:, 0] == pytest.approx(route.v_ego[i])
  assert X[:, 1] == pytest.approx(route.lat_accel[c])


def test_rows_follow_a_lag_that_changes_with_speed(tmp_path):
  # lagd's speed bins change the published delay mid-drive; each row has to use the one in effect
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 40.0, phase=0.0, lag_at=lambda t: 0.25 if t < 20.0 else 0.4)
  route = T.assemble_route("r", [T.extract_segment(path)])
  X, y = T.build_rows(route, stride=1)
  t = T.row_indices(route, stride=1) * T.DT
  away = np.abs(t - 20.0) > 0.5
  assert np.sum(away & (t < 20.0)) > 1000 and np.sum(away & (t > 20.0)) > 1000
  assert y[away] == pytest.approx(plant_torque(X[away, 1], X[away, 3]), abs=5e-3)


def test_rows_skip_windows_that_touch_a_disengagement(tmp_path):
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 30.0, phase=0.0, disengaged=(15.0, 16.0))
  route = T.assemble_route("r", [T.extract_segment(path)])
  disengaged = np.flatnonzero(~route.ok[1:]) + 1  # the first grid point predates the first carControl
  assert disengaged.min() == pytest.approx(15.0 / T.DT, abs=2)
  assert disengaged.max() == pytest.approx(16.0 / T.DT, abs=2)

  i = T.row_indices(route, stride=1)
  post, pre = int(round(LAG / T.DT)) + max(T.HISTORY_FRAMES), int(T.ENGAGED_BEFORE / T.DT)
  before, after = i[i + post < disengaged.min()], i[i - pre > disengaged.max()]
  assert len(before) + len(after) == len(i)
  # and nothing more than the overlapping windows is lost
  assert before.max() == disengaged.min() - post - 1
  assert after.min() == disengaged.max() + pre + 1


def test_truncated_rlog_still_reads(tmp_path):
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 10.0, phase=0.0)
  full = len(T.extract_segment(path)["cs_t"])
  path.write_bytes(path.read_bytes()[:-2000])
  partial = len(T.extract_segment(path)["cs_t"])
  assert 0 < partial < full


@pytest.mark.parametrize("huber_delta", [10.0, 0.05])
def test_gradients_match_finite_differences(huber_delta):
  rng = np.random.default_rng(0)
  model = T.MLP([5, 4, 3, 1], rng)
  X, y, w = rng.normal(size=(64, 5)), rng.normal(size=64), rng.uniform(0.5, 2.0, size=64)
  _, gW, gb = model.loss_and_grads(X, y, w, huber_delta)
  for params, grads in ((model.W, gW), (model.b, gb)):
    for p, g in zip(params, grads, strict=True):
      for idx in [(0,) * p.ndim, tuple(s - 1 for s in p.shape)]:
        old = p[idx]
        p[idx] = old + 1e-6
        up = model.loss_and_grads(X, y, w, huber_delta)[0]
        p[idx] = old - 1e-6
        down = model.loss_and_grads(X, y, w, huber_delta)[0]
        p[idx] = old
        assert g[idx] == pytest.approx((up - down) / 2e-6, rel=1e-4, abs=1e-8)


def test_exported_model_evaluates_the_same_in_flux_model(tmp_path):
  rng = np.random.default_rng(1)
  X = rng.normal(size=(4000, len(T.INPUT_VARS)))
  X[:, 0] = rng.uniform(5, 35, size=len(X))
  y = np.tanh(X[:, 1]) - 0.3 * X[:, 3]
  w = np.ones(len(X))
  result = T.train(X[:3000], y[:3000], w[:3000], X[3000:], y[3000:], w[3000:], hidden=(8, 6), epochs=3, verbose=False)
  T.export_model(result, tmp_path / "CAR.json")

  flux = FluxModel(tmp_path / "CAR.json")
  rows = X[3000:3020]
  assert [flux.evaluate(list(row)) for row in rows] == pytest.approx(T.predict(result, rows), abs=1e-4)
  assert flux.friction_override == T.friction_override_triggers(result)


def test_trains_a_model_that_beats_linear_on_held_out_routes(log_dir, tmp_path):
  routes = [T.assemble_route(name, [T.extract_segment(p) for _, p in segs]) for name, segs in T.find_segments([log_dir]).items()]
  (X_train, y_train), (X_val, y_val) = [np.concatenate(a) for a in zip(*[T.build_rows(r) for r in routes[:2]], strict=True)], T.build_rows(routes[2])
  X_train, y_train = T.mirror(X_train, y_train)
  result = T.train(X_train, y_train, T.balance_weights(X_train[:, 1]), X_val, y_val, T.balance_weights(X_val[:, 1]),
                   epochs=40, verbose=False)

  nn = T.metrics(y_val, T.predict(result, X_val))["rmse"]
  k = np.linalg.lstsq((X_train[:, 1] - T.GRAVITY * X_train[:, 3])[:, None], y_train, rcond=None)[0][0]
  linear = T.metrics(y_val, k * (X_val[:, 1] - T.GRAVITY * X_val[:, 3]))["rmse"]
  assert nn < 0.5 * linear
  assert nn < 0.03


def test_cli_writes_a_loadable_model(log_dir, tmp_path, capsys):
  out = tmp_path / "MAZDA_CX_30.json"
  T.main([str(log_dir), "--out", str(out), "--epochs", "15", "--workers", "1", "--no-cache", "--siglin", *map(str, PLANT)])
  printed = capsys.readouterr().out
  assert "MAZDA_CX_30: 3 routes" in printed
  assert "siglin" in printed
  model = FluxModel(out)
  assert model.evaluate([20.0, 1.0, 0.0, 0.0] + [1.0] * 7 + [0.0] * 7) > 0.0


def fake_log(monkeypatch, events):
  """Stand in for the cereal schema, handing extract_segment these events instead of a decoded rlog."""
  monkeypatch.setattr(T, "load_log_schema", lambda cereal_dir=None: SimpleNamespace(
    Event=SimpleNamespace(read_multiple_bytes=lambda data: iter(events))))
  monkeypatch.setattr(T, "read_log_bytes", lambda path: b"")


def fake_event(which, t, **fields):
  return SimpleNamespace(which=lambda: which, logMonoTime=int(t * 1e9), **{which: SimpleNamespace(**fields)})


CAR_STATE = dict(vEgo=20.0, steeringPressed=False, steerFaultTemporary=False, steerFaultPermanent=False)


def assert_columns_line_up(seg):
  for t_key, _, value_keys in T.REQUIRED_SERVICES:
    for k in value_keys:
      assert len(seg.get(k, [])) == len(seg.get(t_key, []))


def test_a_message_that_fails_midway_leaves_every_column_the_same_length(monkeypatch):
  class Torque:  # a carOutput whose torque can't be read, like a message cut short
    @property
    def torque(self):
      raise RuntimeError("Message ends prematurely")

  fake_log(monkeypatch, [fake_event("carState", 1.0, **CAR_STATE), fake_event("carState", 1.01, **CAR_STATE),
                         SimpleNamespace(which=lambda: "carOutput", logMonoTime=int(1.02e9), carOutput=SimpleNamespace(actuatorsOutput=Torque()))])
  seg = T.extract_segment("rlog.zst")
  assert len(seg["cs_t"]) == 2
  assert_columns_line_up(seg)  # co_t has no stray entry without a torque to go with it


def test_a_truncated_rlog_keeps_everything_before_the_cut(tmp_path):
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 5.0, phase=0)
  raw = zstandard.ZstdDecompressor().stream_reader(path.read_bytes()).read()
  path.write_bytes(zstandard.ZstdCompressor().compress(raw[:-40]))  # mid-message
  seg = T.extract_segment(path)
  assert len(seg["cs_t"]) > 400
  assert_columns_line_up(seg)


def test_logs_with_fields_the_schema_lacks_raise_instead_of_looking_truncated(monkeypatch):
  class OldOutput:  # a cereal where Actuators has no torque
    @property
    def actuatorsOutput(self):
      raise AttributeError("struct has no such member: torque")

  fake_log(monkeypatch, [fake_event("carState", 1.0, **CAR_STATE),
                         SimpleNamespace(which=lambda: "carOutput", logMonoTime=int(1.01e9), carOutput=OldOutput())])
  with pytest.raises(AttributeError, match="torque"):
    T.extract_segment("rlog.zst")


def test_a_route_missing_a_service_is_skipped_and_says_what_its_logs_do_hold(tmp_path):
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 5.0, phase=0)
  seg = T.extract_segment(path)
  for key in ("cs_t", "v_ego", "steering_pressed", "steer_fault"):
    del seg[key]
  skipped = []
  assert T.assemble_route("r", [seg, seg], skipped) is None
  route, service, detail = skipped[0]
  assert (route, service) == ("r", "carState")
  assert "carOutput x" in detail


def test_columns_that_dont_line_up_skip_the_route_instead_of_crashing(tmp_path):
  # what a cache written by an older version of the script could hand back
  path = tmp_path / "00000000--abcdef0--0" / "rlog.zst"
  write_segment(path, 0.0, 5.0, phase=0)
  seg = T.extract_segment(path)
  del seg["torque"]
  skipped = []
  assert T.assemble_route("r", [seg, seg], skipped) is None
  assert skipped[0][1] == "carOutput"
  assert "--no-cache" in skipped[0][2]
