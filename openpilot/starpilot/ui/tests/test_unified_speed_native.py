"""One set speed and a shared, latest-limit sign in the large and compact UIs."""
from dataclasses import replace
from types import SimpleNamespace
from unittest.mock import Mock

import pyray as rl
import pytest

from openpilot.starpilot.ui.onroad_compact_widgets import CompactHudRenderer
from openpilot.starpilot.ui.onroad_large_widgets import UnifiedSpeedWidget
from openpilot.starpilot.ui.onroad_state import ObservationKind, OnroadState, SpeedLimitObservation
from openpilot.starpilot.ui.presentation import Profile
from openpilot.starpilot.ui.speed_limit_widget import SpeedLimitWidget
from openpilot.starpilot.ui.unified_speed_presentation import displayed_limit_mps, resolve_unified_speed


@pytest.fixture(autouse=True)
def drawing(monkeypatch):
  calls = {}
  for name in ('draw_rectangle_rounded_lines_ex', 'draw_rectangle_rounded', 'draw_line_ex', 'draw_circle', 'draw_ring'):
    calls[name] = Mock()
    monkeypatch.setattr(rl, name, calls[name])
  monkeypatch.setattr('openpilot.starpilot.ui.onroad_large_widgets.draw_control_card', Mock())
  return calls


def state(**changes):
  observation = SpeedLimitObservation(ObservationKind.VALID, 'map', 50 / 3.6,
    accepted_speed_limit_mps=40 / 3.6, accepted_adjusted_limit_mps=45 / 3.6,
    pending_speed_limit_mps=50 / 3.6, pending_adjusted_limit_mps=55 / 3.6,
    presentation_adjusted_limit_mps=55 / 3.6, accepted_source='dashboard', action_enabled=True,
    session_id='drive', presentation_id=2, decision_id=1, pending_since_ns=10_000_000_000)
  return replace(OnroadState(True, False, 45 / 3.6, 100., observation, metric=True, longitudinal_active=True,
                              observed_ns=10_000_000_000), **changes)


def fonts():
  return SimpleNamespace(draw=Mock(), measure=lambda text, role, size: SimpleNamespace(width=len(text) * size * .5, height=size))


def test_latest_limit_and_offset_replace_accepted_sign_while_waiting():
  shown = resolve_unified_speed(state())
  assert (shown.max_text, shown.posted_text, shown.offset_text, shown.source) == ('100', '50', '+5', 'map')
  assert displayed_limit_mps(state().speed_limit) == 50 / 3.6


@pytest.mark.parametrize('profile', list(Profile))
def test_both_renderers_show_small_latest_sign_and_offset(profile):
  font = fonts()
  current = state()
  if profile == Profile.LARGE:
    UnifiedSpeedWidget(font).render(rl.Rectangle(30, 30, 1800, 1020), current)
    texts = [call.args[0] for call in font.draw.call_args_list]
    assert texts.count('100') == 1
    assert 'SET SPEED' in texts
    assert 'SLC LIMIT' not in texts
  else:
    CompactHudRenderer(font, Mock())._speed_limit_sign(current)
  texts = [call.args[0] for call in font.draw.call_args_list]
  assert '50' in texts and '+5' in texts and '40' not in texts


@pytest.mark.parametrize('profile', list(Profile))
def test_pending_sign_hops_and_arrow_follows_vehicle_speed(profile, drawing):
  font = fonts()
  sign = SpeedLimitWidget(font, profile)
  current = state()
  sign.render(current)
  y = drawing['draw_rectangle_rounded_lines_ex'].call_args.args[0].y
  arrow = drawing['draw_line_ex'].call_args_list[0].args
  assert arrow[1].y < arrow[0].y
  sign.render(replace(current, observed_ns=10_750_000_000, speed_mps=60 / 3.6))
  assert drawing['draw_rectangle_rounded_lines_ex'].call_args.args[0].y == y - 6
  arrow = drawing['draw_line_ex'].call_args_list[-3].args
  assert arrow[1].y > arrow[0].y


@pytest.mark.parametrize('profile', list(Profile))
def test_only_set_speed_override_draws_cross(profile, drawing):
  sign = SpeedLimitWidget(fonts(), profile)
  current = state()
  current = replace(current, speed_limit=replace(current.speed_limit, pending_speed_limit_mps=None))
  sign.render(replace(current, longitudinal_overridden=True))
  assert drawing['draw_line_ex'].call_count == 0
  sign.render(replace(current, speed_limit=replace(current.speed_limit, driver_override_active=True)))
  assert drawing['draw_line_ex'].call_count == 2


def test_compact_set_speed_stays_visible_while_cruise_is_active():
  renderer = CompactHudRenderer(fonts(), Mock())
  current = state(drive_frame=1)
  for now in range(10_000_000_000, 20_000_000_000, 100_000_000):
    alpha = renderer._set_speed_opacity(current, now)
  assert alpha > .99
  assert renderer._set_speed_opacity(replace(current, longitudinal_active=False), 20_000_000_000) == 0
