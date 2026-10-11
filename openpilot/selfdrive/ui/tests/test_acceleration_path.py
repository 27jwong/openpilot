import unittest
from types import SimpleNamespace
from unittest.mock import Mock, patch

import numpy as np

import openpilot.selfdrive.ui.mici.onroad.model_renderer as mici_model_renderer
import openpilot.selfdrive.ui.onroad.model_renderer as model_renderer
from openpilot.selfdrive.ui.ui_state import UIStatus
from openpilot.system.ui.lib.shader_polygon import Gradient


def _make_renderer(module, experimental_mode):
  renderer = object.__new__(module.ModelRenderer)
  renderer._experimental_mode = experimental_mode
  renderer._longitudinal_control = True
  renderer._rect = SimpleNamespace(x=0.0, y=0.0, width=100.0, height=100.0)
  renderer._rainbow_path = SimpleNamespace(refresh_enabled=Mock(return_value=False))
  renderer._visual_status = Mock(return_value=UIStatus.ENGAGED)
  renderer.road_style = {"pathMode": "acceleration"}

  # Left edge runs bottom to top, right edge back down, as _map_line_to_polygon emits them
  left = np.column_stack((np.full(10, 40.0), np.linspace(95.0, 50.0, 10)))
  right = np.column_stack((np.full(10, 60.0), np.linspace(50.0, 95.0, 10)))
  renderer._path = SimpleNamespace(projected_points=np.vstack((left, right)).astype(np.float32))
  renderer._acceleration_x = np.linspace(1.5, -1.5, 10, dtype=np.float32)
  renderer._exp_gradient = Gradient(start=(0.0, 1.0), end=(0.0, 0.0), colors=[], stops=[])
  return renderer


class TestAccelerationPath(unittest.TestCase):
  def test_acceleration_path_colors_path_in_every_mode(self):
    for module in (model_renderer, mici_model_renderer):
      for experimental_mode in (False, True):
        with self.subTest(module=module.__name__, experimental_mode=experimental_mode):
          with (patch.object(module, "draw_polygon") as draw_polygon,
                patch.object(module, "ui_state", SimpleNamespace(params=object()))):
            renderer = _make_renderer(module, experimental_mode)
            renderer._update_experimental_gradient()
            renderer._draw_path({"longitudinalPlan": SimpleNamespace(allowThrottle=False)})

          colors = renderer._exp_gradient.colors
          self.assertGreater(len(colors), 1)
          self.assertEqual([c.kwargs.get("gradient") for c in draw_polygon.call_args_list], [renderer._exp_gradient])

          # Speeding up near the car reads green, slowing down further out reads red
          self.assertGreater(colors[0].g, colors[0].r)
          self.assertGreater(colors[-1].r, colors[-1].g)


if __name__ == "__main__":
  unittest.main()
