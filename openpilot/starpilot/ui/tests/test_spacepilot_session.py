"""Exclusive game admission, real drive-state requests, and unsafe lifecycle exits."""

from types import SimpleNamespace as NS
from unittest.mock import Mock

import pytest

from openpilot.common.params import Params
from openpilot.starpilot.drive_state.owner import DriveStateOwner
from openpilot.starpilot.ui.spacepilot import session
from openpilot.starpilot.ui.spacepilot.guard import DRIVING_PROCESSES, read_offroad_witness
from openpilot.starpilot.ui.tests.test_spacepilot_state import parked_ui


class Clock:
  now = 10_000_000_000
  offset = 2_000_000_000

  def refresh(self, ui, step=100_000_000):
    self.now += step
    for name in ui.sm:
      ui.sm.valid[name] = ui.sm.alive[name] = True
      ui.sm.recv_frame[name] = 10
      ui.sm.recv_time[name] = self.now / 1e9
      ui.sm.logMonoTime[name] = self.now + (self.offset if name == 'pandaStates' else 0)


class Vehicle:
  def __init__(self, cp):
    self.cp = cp
    self.fresh = True
    self.closed = False
    self.sample = NS(source_boot_ns=1)
    self.samples = ()

  def poll(self):
    result, self.samples = self.samples, ()
    return result

  def ready(self, *_):
    return self.fresh and not self.closed

  def close(self):
    self.closed = True


def game_ui(*, ignition=True):
  ui = parked_ui()
  ui.params = NS(get_bool=lambda _: not ui.started)
  ui.sm['deviceState'].thermalStatus = 'ok'
  ui.sm['managerState'] = NS(processes=[NS(name=name, running=True) for name in DRIVING_PROCESSES])
  ui.sm['carParams'] = ui.CP
  ui.sm['pandaStates'][0].safetyModel = 'hyundai'
  ui.sm['pandaStates'][0].ignitionLine = ignition
  return ui


def offroad(ui):
  ui.started = ui.sm['deviceState'].started = False
  for process in ui.sm['managerState'].processes:
    process.running = False
  ui.sm['pandaStates'][0].safetyModel = 'noOutput'


@pytest.fixture
def setup(monkeypatch, tmp_path):
  clock = Clock()
  ui = game_ui()
  clock.refresh(ui, 0)
  params = Params(str(tmp_path / 'params'))
  owner = DriveStateOwner(params, tmp_path / 'drive-state')
  owner.initialize_manager()
  monkeypatch.setattr(session.GameSession, '_clocks', lambda _: (clock.now, clock.now + clock.offset))
  game = session.GameSession(ui, owner, native_device=True, reader_factory=Vehicle)
  game.started_ns = clock.now
  yield game, ui, owner, clock
  game.close()


def enter(game, ui, clock, *, request=True):
  for _ in range(12):
    game.step()
    clock.refresh(ui)
  if request:
    assert game.stage == session.Stage.STOPPING
    assert game.requested_offroad
  offroad(ui)
  for _ in range(12):
    game.step()
    clock.refresh(ui)
  assert game.stage == session.Stage.PLAYING, game.reason


def test_actual_owner_request_waits_for_processes_nooutput_then_stays_offroad(setup):
  game, ui, owner, clock = setup
  initial = owner.snapshot()
  for _ in range(12):
    assert not game.step()
    clock.refresh(ui)
  assert owner.snapshot().mode == 'offroad' and owner.snapshot().revision != initial.revision
  assert game.stage == session.Stage.STOPPING and not game.playable()
  ui.started = ui.sm['deviceState'].started = False
  for _ in range(12):
    game.step()
    clock.refresh(ui)
    assert not game.playable()
  offroad(ui)
  for _ in range(12):
    game.step()
    clock.refresh(ui)
  assert game.playable()
  vehicle = game.vehicle
  state = owner.snapshot()
  game.close()
  assert vehicle.closed and game.vehicle is None
  assert owner.snapshot() == state and state.mode == 'offroad'


def test_offroad_ignition_off_touch_never_creates_vehicle_reader_or_changes_mode(setup):
  game, ui, owner, clock = setup
  ui.sm['pandaStates'][0].ignitionLine = False
  offroad(ui)
  initial = owner.snapshot()
  enter(game, ui, clock, request=False)
  assert game.vehicle is None and game.playable()
  assert owner.snapshot() == initial
  assert not game.wheel.armed


def test_offroad_with_ignition_requires_independent_fresh_park(setup):
  game, ui, owner, clock = setup
  offroad(ui)
  game.step()
  game.vehicle.fresh = False
  for _ in range(20):
    clock.refresh(ui)
    game.step()
  assert not game.playable() and game.stage == session.Stage.VERIFYING
  game.vehicle.fresh = True
  enter(game, ui, clock, request=False)
  assert game.playable()
  assert owner.snapshot().mode == 'auto'


@pytest.mark.parametrize('change', ['source', 'receiver', 'future', 'invalid', 'dead'])
@pytest.mark.parametrize('service', ['deviceState', 'pandaStates', 'managerState'])
def test_active_game_exits_for_every_lost_transport(setup, service, change):
  game, ui, _, clock = setup
  enter(game, ui, clock)
  if change == 'source':
    ui.sm.logMonoTime[service] -= 2_000_000_000
  elif change == 'receiver':
    ui.sm.recv_time[service] -= 2
  elif change == 'future':
    ui.sm.logMonoTime[service] += 2_000_000_000
  else:
    getattr(ui.sm, 'valid' if change == 'invalid' else 'alive')[service] = False
  assert not game.playable()
  game.step()
  assert game.stage == session.Stage.CLOSED and game.vehicle is None


@pytest.mark.parametrize('change', ['ignition', 'controls', 'safety', 'fault', 'harness', 'process', 'missing_process',
                                   'duplicate_process', 'started', 'raw_stale', 'thermal', 'drive_mode'])
def test_unsafe_exit_and_no_automatic_restore(setup, change):
  game, ui, owner, clock = setup
  enter(game, ui, clock)
  panda = ui.sm['pandaStates'][0]
  if change == 'ignition':
    panda.ignitionLine = False
  elif change == 'controls':
    panda.controlsAllowed = True
  elif change == 'safety':
    panda.safetyModel = 'hyundai'
  elif change == 'fault':
    panda.heartbeatLost = True
  elif change == 'harness':
    panda.harnessStatus = 'notConnected'
  elif change == 'process':
    ui.sm['managerState'].processes[0].running = True
  elif change == 'missing_process':
    ui.sm['managerState'].processes.pop()
  elif change == 'duplicate_process':
    ui.sm['managerState'].processes.append(ui.sm['managerState'].processes[0])
  elif change == 'started':
    ui.started = ui.sm['deviceState'].started = True
  elif change == 'thermal':
    ui.sm['deviceState'].thermalStatus = 'overheated'
  elif change == 'raw_stale':
    game.vehicle.fresh = False
  else:
    owner.request('auto', expected_revision=owner.snapshot().revision, authorized=lambda: True, override_allowed=lambda: True)
  expected = owner.snapshot()
  assert not game.playable()
  game.step()
  assert game.stage == session.Stage.CLOSED
  assert owner.snapshot() == expected


def test_wheel_requires_post_admission_release_and_does_not_repeat(setup):
  game, ui, _, clock = setup
  enter(game, ui, clock)
  def edge(pressed):
    clock.refresh(ui, 140_000_000)
    game.vehicle.samples = (NS(source_boot_ns=clock.now + clock.offset, buttons=(('accelCruise', pressed),)),)
    return game.step()
  assert not edge(True)
  assert not edge(False)
  assert edge(True)
  assert not edge(True)
  assert not edge(False)
  assert edge(True)


def test_current_vehicle_identity_required_and_forced_flag_alone_never_admits(setup):
  game, ui, owner, clock = setup
  initial = owner.snapshot()
  ui.sm.recv_frame['carParams'] = ui.started_frame
  game.step()
  assert game.stage == session.Stage.CLOSED and owner.snapshot() == initial
  offroad(ui)
  assert read_offroad_witness(ui, clock.now, clock.now + clock.offset, native_device=True) is None
  assert read_offroad_witness(ui, clock.now, clock.now + clock.offset, native_device=True, preparing=True) is not None


def test_pending_transition_timeout_and_constructor_error_cleanup(setup):
  game, ui, owner, clock = setup
  game._reader_factory = Mock(side_effect=ValueError('Unsupported decoder'))
  game.step()
  assert game.stage == session.Stage.CLOSED and owner.snapshot().mode == 'auto'
  timed = session.GameSession(ui, owner, native_device=True, reader_factory=Vehicle)
  timed.started_ns = clock.now - session.TRANSITION_NS - 1
  timed.step()
  assert timed.stage == session.Stage.CLOSED and timed.vehicle is None


def test_device_resume_invalidates_session_clock(monkeypatch):
  game = session.GameSession(NS(), NS(), native_device=True)
  mono, boot = [10_000_000_000], [12_000_000_000]
  monkeypatch.setattr(session.time, 'monotonic_ns', lambda: mono[0])
  monkeypatch.setattr(session.time, 'clock_gettime_ns', lambda _: boot[0])
  assert game._clocks() == (mono[0], boot[0])
  boot[0] += 6_000_000
  with pytest.raises(ValueError, match='resumed'):
    game._clocks()


def test_offroad_witness_accepts_actual_schema_and_refuses_actual_thermal_faults(setup):
  from openpilot.cereal import messaging
  game, ui, _, clock = setup
  offroad(ui)
  ui.sm['pandaStates'][0].ignitionLine = False
  for name in ('deviceState', 'pandaStates', 'managerState'):
    event = messaging.new_message(name, 1 if name == 'pandaStates' else None, valid=True)
    if name == 'pandaStates':
      event.pandaStates[0] = vars(ui.sm[name][0])
    elif name == 'managerState':
      event.managerState.processes = [vars(p) for p in ui.sm[name].processes]
    else:
      event.deviceState = vars(ui.sm[name])
    ui.sm[name] = getattr(event.as_reader(), name)
  assert read_offroad_witness(ui, clock.now, clock.now + clock.offset, native_device=True) is not None
  for thermal in ('overheated', 'critical'):
    message = ui.sm['deviceState'].as_builder()
    message.thermalStatus = thermal
    ui.sm['deviceState'] = message.as_reader()
    assert read_offroad_witness(ui, clock.now, clock.now + clock.offset, native_device=True) is None
