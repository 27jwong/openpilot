from dataclasses import replace
from unittest.mock import Mock, patch

import pytest

from openpilot.starpilot.ui.large_speed_geometry import LARGE_SIGN_SCALE
from openpilot.starpilot.ui.onroad_state import speed_limit_bounds, slc_controls, compact_sign_obscured_by_actions, SpeedLimitObservation, ObservationKind
from openpilot.starpilot.ui.presentation import Profile
from openpilot.starpilot.ui.speed_limit_widget import SpeedLimitWidget
from openpilot.starpilot.ui.tests.test_unified_speed_native import fonts, state
from openpilot.starpilot.ui.unified_speed_presentation import resolve_unified_speed


@pytest.mark.parametrize('profile', list(Profile))
@pytest.mark.parametrize('offset', [False, True])
@pytest.mark.parametrize('vienna', [False, True])
def test_only_small_rectangular_sign_and_touch_bounds_shrink_without_offset(profile, offset, vienna):
  current = state(show_slc_offset=offset)
  current = replace(current, appearance=replace(current.appearance, use_vienna_sign=vienna))
  widget = SpeedLimitWidget(fonts(profile), profile)
  widget.shapes = Mock()
  with patch('openpilot.starpilot.ui.speed_limit_widget.rl.draw_rectangle_rounded') as fill:
    widget.render(current)
  fill.assert_not_called()
  scale = LARGE_SIGN_SCALE if profile == Profile.LARGE else 1
  bounds = speed_limit_bounds(profile, current)
  tall = speed_limit_bounds(profile, replace(current, show_slc_offset=True))
  assert bounds is not None and tall is not None
  short = profile == Profile.COMPACT and not vienna and not offset
  assert bounds[:3] == tall[:3]
  assert bounds[3] == tall[3] - (16 if short else 0)
  if vienna:
    widget.shapes.outline.assert_not_called()
    widget.shapes.circle.assert_called_once()
  else:
    widget.shapes.outline.assert_called_once()
    assert widget.shapes.outline.call_args.args[:2] == (bounds[0]+12+59*scale, bounds[1]+12+(58 if short else 66)*scale)
    assert widget.shapes.outline.call_args.kwargs == ({'short': True} if short else {})
  assert ('+5' in [call.args[0] for call in widget.fonts.draw.call_args_list]) == offset


@pytest.mark.parametrize('value', (65, 120))
@pytest.mark.parametrize('metric', (False, True))
@pytest.mark.parametrize('show', (False, True))
@pytest.mark.parametrize('offset', (None, 0., .01, -.01, 5., -10.))
def test_small_sign_has_space_only_for_a_visible_offset(value, metric, show, offset):
  factor = 3.6 if metric else 2.2369362921
  current = state(metric=metric, show_slc_offset=show)
  current = replace(current, appearance=replace(current.appearance, show_speed_limit_sign=True, use_vienna_sign=False),
    speed_limit=SpeedLimitObservation(ObservationKind.VALID, 'vision', value / factor,
    offset_mps=None if offset is None else offset / factor, action_enabled=False))
  shown = resolve_unified_speed(current)
  widget = SpeedLimitWidget(fonts(Profile.COMPACT), Profile.COMPACT)
  widget.shapes = Mock()
  widget.render(current)
  bounds = speed_limit_bounds(Profile.COMPACT, current)
  assert bounds is not None
  short = not shown.offset_text
  assert widget.shapes.outline.call_args.kwargs == ({'short': True} if short else {})
  assert bounds[3] - bounds[1] == (136 if short else 152)
  number = next(call.args for call in widget.fonts.draw.call_args_list if call.args[0] == str(value))
  assert number[4] == bounds[1] + 12 + 50
  assert number[2] == 58


def test_small_offset_layout_transitions_reuse_the_same_widget():
  widget = SpeedLimitWidget(fonts(Profile.COMPACT), Profile.COMPACT)
  widget.shapes = Mock()
  for enabled in (False, True, False):
    current = state(show_slc_offset=enabled)
    widget.render(current)
    assert widget.shapes.outline.call_args.kwargs == ({} if enabled else {'short': True})


@pytest.mark.parametrize('y', [20, 50, 108])
def test_compact_confirmation_overlap_remains_conservative(y):
  current = state(slc_prompt_open=True)
  current.customization['layouts']['compact']['speed_limit']['y'] = y
  obscured = compact_sign_obscured_by_actions(current)
  assert obscured is (y == 108)
  assert (speed_limit_bounds(Profile.COMPACT, current) is None) is obscured
  assert slc_controls(Profile.COMPACT, current)


def test_small_rectangular_no_offset_regression():
  current = state(show_slc_offset=False)
  widget = SpeedLimitWidget(fonts(Profile.COMPACT), Profile.COMPACT)
  widget.shapes = Mock()
  widget.render(current)
  bounds = speed_limit_bounds(Profile.COMPACT, current)
  assert bounds is not None and bounds[3] - bounds[1] == 136
  assert widget.shapes.outline.call_args.kwargs == {'short': True}
