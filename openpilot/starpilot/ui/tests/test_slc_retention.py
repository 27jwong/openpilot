"""Saved retention policy through Maps, the native planner, and the driving UI."""

from dataclasses import replace
from pathlib import Path
from unittest.mock import Mock, patch

import pytest

from openpilot.common.params import Params
from openpilot.selfdrive.controls.lib import longitudinal_planner as native
from openpilot.starpilot.speed_limits.runtime import Action, Runtime
from openpilot.starpilot.speed_limits.runtime_settings import read_params
from openpilot.starpilot.speed_limits.tests.test_maps_runtime import BOOT, packet
from openpilot.starpilot.ui.onroad_compact_widgets import CompactHudRenderer
from openpilot.starpilot.ui.onroad_state import ObservationKind, slc_controls
from openpilot.starpilot.ui.presentation import Profile
from openpilot.starpilot.ui.shell import ShellMode
from openpilot.starpilot.ui.tests.test_runtime_snapshot import NOW, RuntimeSnapshotAdapter, ui_fake
from openpilot.starpilot.ui.tests.test_slc_ui_runtime import replay_inputs
from openpilot.starpilot.ui.unified_speed_presentation import resolve_unified_speed

MPH = .44704


class RetentionDrive:
  def __init__(self, path, *, keep=True, confirm=False, dashboard=False):
    params = Params(str(path))
    for key, value in {'SpeedLimitController': True, 'SLCConfirmation': confirm,
                       'SLCConfirmationHigher': False, 'SLCConfirmationLower': False,
                       'SLCPriority1': 'Map Data', 'SLCPriority2': 'Dashboard' if dashboard else 'Map Data',
                       'SLCFallback': 2 if keep else 0}.items():
      params.put(key, value, block=True)
    self.runtime = Runtime(read_params(params), session_id='retention-drive')
    assert not self.runtime.settings.errors
    self.sm, self.cp = replay_inputs()
    car = self.sm['carState']
    car.vEgo = car.vEgoCluster = 45 * MPH
    car.vCruise = car.vCruiseCluster = 70 * MPH * 3.6
    self.sm['slcDashboardObservation'].speedMps = 70 * MPH
    self.ui = ui_fake()
    self.ui.CP = self.cp
    self.ui.sm['carState'].canValid, self.ui.sm['carState'].canTimeout = True, False
    self.ui.sm['carControl'].enabled = self.ui.sm['selfdriveState'].enabled = True
    self.ui.sm['carControl'].longActive = True
    self.adapter = RuntimeSnapshotAdapter(self.ui)
    self.planner = native.LongitudinalPlanner(self.cp, init_v=45 * MPH)

  def step(self, milliseconds, *, received=True, alive=True, status='matchedLimit', limit=45, request=None, **changes):
    now, boot = NOW + milliseconds * 1_000_000, BOOT + milliseconds * 1_000_000
    previous_stamp = self.sm.logMonoTime.get('mapdOut', BOOT)
    self.sm.advance(now)
    dashboard = self.sm['slcDashboardObservation']
    dashboard.observedMonoTime, dashboard.validUntilMonoTime = now - 1, now + 1_000_000_000
    if received:
      event = packet(speedLimit=limit * MPH, computedMonoTime=boot, gpsMonoTime=boot - 100_000_000)
      event.logMonoTime, event.mapdOut.roadStatus = boot, status
      if status in ('noGps', 'noCoverage', 'noMatch'):
        event.valid = False
        event.mapdOut.speedLimit, event.mapdOut.wayId, event.mapdOut.tileLoaded = 0., 0, False
      if status == 'noGps':
        event.mapdOut.gpsSource, event.mapdOut.gpsMonoTime, event.mapdOut.sourceGeneration = 'none', 0, 2
      if status == 'matchedNoLimit':
        event.mapdOut.speedLimit = 0.
      for field, value in changes.items():
        setattr(event.mapdOut, field, value)
      self.sm.data['mapdOut'] = event.mapdOut
      self.sm.valid['mapdOut'], self.sm.logMonoTime['mapdOut'] = bool(event.valid), boot
    else:
      self.sm.logMonoTime['mapdOut'] = previous_stamp
    self.sm.updated['mapdOut'], self.sm.alive['mapdOut'] = received, alive
    output = self.runtime.step(self.sm, self.cp, now_ns=now, now_boot_ns=boot, request=request)
    self.ui.sm.put('slcState', output.message.slcState)
    self.ui.sm['carState'].vCruiseCluster = self.sm['carState'].vCruiseCluster
    for service in ('slcState', 'carState', 'carControl', 'selfdriveState'):
      self.ui.sm.logMonoTime[service] = now
    view = self.adapter.build(ShellMode.ONROAD, now_ns=now).onroad
    with patch.object(native, 'get_cruise_accel', wraps=native.get_cruise_accel) as cruise:
      self.planner.update(self.sm, cruise_ceiling=output.result.ceiling, now_ns=now)
    return output, view, cruise.call_args.args[1] / MPH


@pytest.mark.parametrize('keep', [False, True])
@pytest.mark.parametrize('loss', ['noCoverage', 'noMatch', 'noGps', 'matchedNoLimit', 'missingTile',
                                 'lookahead', 'stopped', 'deadTransport', 'externalGpsStale'])
def test_maps_loss_retains_only_accepted_limit_when_enabled(tmp_path, keep, loss):
  drive = RetentionDrive(tmp_path, keep=keep)
  drive.step(0)
  changes = {'status': loss}
  if loss in ('stopped', 'deadTransport'):
    changes = {'received': False, 'alive': loss == 'stopped'}
  elif loss == 'missingTile':
    changes = {'tileLoaded': False}
  elif loss == 'lookahead':
    changes = {'waySelectionType': 'possible'}
  elif loss == 'externalGpsStale':
    changes = {'gpsMonoTime': BOOT - 300_000_000}
  output, view, target = drive.step(251, **changes)
  assert target == pytest.approx(45 if keep else 70)
  assert output.message.slcState.acceptedSpeedLimit == pytest.approx(45 * MPH)
  assert output.message.slcState.hasCeiling == keep
  assert output.message.slcState.hasAcceptedAdjustedLimit
  assert output.message.slcState.acceptedAdjustedLimit == pytest.approx(45 * MPH)
  assert view.speed_limit.kind != ObservationKind.VALID
  shown = resolve_unified_speed(view)
  assert (shown.max_text, shown.posted_text, shown.status) == ('70', '45' if keep else '–',
                                                           'Using previous limit' if keep else 'Unavailable')
  assert all(slc_controls(profile, view) == () for profile in Profile)
  assert not output.message.slcState.hasPending


@pytest.mark.parametrize('answer', ['accept', 'reject', 'timeout'])
@pytest.mark.parametrize('limit', [40, 50])
def test_long_gap_confirmation_and_recovery_keep_the_accepted_limit(tmp_path, answer, limit):
  drive = RetentionDrive(tmp_path, confirm=True)
  first, _, _ = drive.step(0)
  assert first.message.slcState.hasPending and not first.message.slcState.hasAccepted
  drive.step(50, request=Action(drive.runtime.session_id, 1, first.message.slcState.decisionId, 0, 'accept'))
  _, _, target = drive.step(100, status='noCoverage')
  assert target == pytest.approx(45)
  _, view, target = drive.step(35_000, received=False)
  assert target == pytest.approx(45)
  assert resolve_unified_speed(view).status == 'Using previous limit'
  pending, _, target = drive.step(40_000, limit=limit, wayId=124)
  assert target == pytest.approx(45) and pending.message.slcState.hasPending
  action = None if answer == 'timeout' else Action(drive.runtime.session_id, 2, pending.message.slcState.decisionId, 0, answer)
  drive.step(71_000 if answer == 'timeout' else 40_050, limit=limit, wayId=124, request=action)
  output, view, target = drive.step(300_000, received=False)
  assert target == pytest.approx(limit if answer == 'accept' else 45)
  assert not output.message.slcState.hasPending
  assert resolve_unified_speed(view).posted_text == (str(limit) if answer == 'accept' else '45')
  # A dead SLC publisher cannot make the widget claim retained control indefinitely.
  drive.ui.sm.alive['slcState'] = False
  expired = drive.adapter.build(ShellMode.ONROAD, now_ns=NOW + 300_001_000_000).onroad
  assert resolve_unified_speed(expired).active_side == 'none'


def test_unchanged_map_speed_does_not_accept_new_identity_or_keep_old_prompt(tmp_path):
  drive = RetentionDrive(tmp_path, confirm=True)
  first, _, _ = drive.step(0)
  accepted, _, _ = drive.step(50, request=Action(drive.runtime.session_id, 1, first.message.slcState.decisionId, 0, 'accept'))
  unchanged, _, target = drive.step(100, wayId=124)
  assert target == pytest.approx(45)
  assert not unchanged.message.slcState.hasPending
  assert unchanged.result.acceptance.history_write is None
  assert unchanged.result.acceptance.state.accepted == accepted.result.acceptance.state.accepted
  pending, _, _ = drive.step(150, limit=50, wayId=124)
  assert pending.message.slcState.hasPending
  recovered, _, target = drive.step(200)
  assert target == pytest.approx(45) and not recovered.message.slcState.hasPending
  assert not drive.step(250)[0].result.errors


def test_retention_respects_lower_max_secondary_source_and_reset(tmp_path):
  drive = RetentionDrive(tmp_path / 'max')
  drive.step(0)
  drive.sm['carState'].vCruise = drive.sm['carState'].vCruiseCluster = 40 * MPH * 3.6
  _, view, target = drive.step(300, received=False)
  assert target == pytest.approx(40)
  assert resolve_unified_speed(view).status == 'Using set speed'
  drive.runtime.reset()
  output, _, _ = drive.step(400, received=False)
  assert not output.message.slcState.hasAccepted and output.result.ceiling is None
  secondary = RetentionDrive(tmp_path / 'secondary', dashboard=True)
  secondary.step(0)
  output, _, target = secondary.step(300, status='noCoverage')
  assert target == pytest.approx(70)
  assert output.message.slcState.acceptedSource == 'dashboard'


def test_compact_sign_keeps_accepted_value_without_pulsing_on_recovery(tmp_path):
  drive = RetentionDrive(tmp_path)
  _, before, _ = drive.step(0)
  _, held, _ = drive.step(300, received=False)
  _, recovered, _ = drive.step(400)
  fonts = Mock()
  fonts.measure.return_value = Mock(width=20, height=20)
  renderer = CompactHudRenderer(fonts, Path('/unused'))
  with patch('openpilot.starpilot.ui.onroad_compact_widgets.rl.draw_rectangle_rounded_lines_ex'), \
       patch('openpilot.starpilot.ui.onroad_compact_widgets.rl.draw_circle'), \
       patch('openpilot.starpilot.ui.onroad_compact_widgets.rl.draw_ring'):
    for view in (before, held, recovered):
      renderer._speed_limit_sign(replace(view, appearance=replace(view.appearance, show_speed_limit_sign=True)))
  assert any(call.args[0] == '45' for call in fonts.draw.call_args_list)
  assert renderer._speed_limit_pulse.start_time == NOW / 1e9
