from dataclasses import replace
from unittest.mock import patch

import pytest

from openpilot.starpilot.ui.onroad_state import speed_limit_bounds
from openpilot.starpilot.ui.presentation import Profile
from openpilot.starpilot.ui.speed_limit_widget import SpeedLimitWidget
from openpilot.starpilot.ui.tests.test_unified_speed_native import fonts, state


@pytest.mark.parametrize('profile', list(Profile))
@pytest.mark.parametrize('offset', [False, True])
def test_only_compact_rectangle_grows_for_visible_offset(profile, offset):
  current = state(show_slc_offset=offset)
  widget = object.__new__(SpeedLimitWidget)
  widget.fonts = fonts(profile)
  widget.profile = profile
  widget.scale = 1
  widget.text_scale = 1
  widget.pulse = type('Pulse', (), {'color': staticmethod(lambda color, now: color)})()
  bounds = speed_limit_bounds(profile, current)
  with patch('openpilot.starpilot.ui.speed_limit_widget.rl.draw_rectangle_rounded_lines_ex') as outline, \
       patch('openpilot.starpilot.ui.speed_limit_widget.rl.draw_rectangle_rounded'), \
       patch('openpilot.starpilot.ui.speed_limit_widget.SpeedLimitShapes.arrow'):
    from unittest.mock import Mock
    widget.shapes = Mock()
    widget._sign(current, bounds, 10.)
  rectangle = outline.call_args.args[0]
  assert rectangle.height == (112 if profile == Profile.COMPACT and not offset else 128)
  other = speed_limit_bounds(profile, replace(current, show_slc_offset=not offset))
  assert bounds[:3] == other[:3]
  assert abs(bounds[3] - other[3]) == (16 if profile == Profile.COMPACT else 0)
  texts = [call.args[0] for call in widget.fonts.draw.call_args_list]
  assert ('+5' in texts) == offset
