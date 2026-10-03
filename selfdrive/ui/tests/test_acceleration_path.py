from types import SimpleNamespace

import numpy as np
import pytest

import openpilot.selfdrive.ui.mici.onroad.model_renderer as mici_model_renderer
import openpilot.selfdrive.ui.onroad.model_renderer as model_renderer
from openpilot.system.ui.lib.shader_polygon import Gradient


class _FakeParams:
  def get_bool(self, key, default=False):
    return {"RainbowPath": False, "AccelerationPath": True}[key]


def _make_renderer(module, experimental_mode):
  renderer = object.__new__(module.ModelRenderer)
  renderer._experimental_mode = experimental_mode
  renderer._params = _FakeParams()
  renderer._use_rainbow = False
  renderer._use_accel_path = True
  renderer._rect = SimpleNamespace(x=0.0, y=0.0, width=100.0, height=100.0)

  # Left edge runs bottom to top, right edge back down, as _map_line_to_polygon emits them
  left = np.column_stack((np.full(10, 40.0), np.linspace(95.0, 50.0, 10)))
  right = np.column_stack((np.full(10, 60.0), np.linspace(50.0, 95.0, 10)))
  renderer._path = SimpleNamespace(projected_points=np.vstack((left, right)).astype(np.float32))
  renderer._acceleration_x = np.linspace(1.5, -1.5, 10, dtype=np.float32)
  renderer._exp_gradient = Gradient(start=(0.0, 1.0), end=(0.0, 0.0), colors=[], stops=[])
  return renderer


@pytest.mark.parametrize("module", [model_renderer, mici_model_renderer], ids=["big", "mici"])
@pytest.mark.parametrize("experimental_mode", [False, True], ids=["chill", "experimental"])
def test_acceleration_path_colors_path_in_every_mode(monkeypatch, module, experimental_mode):
  drawn = []
  monkeypatch.setattr(module, "draw_polygon", lambda *args, **kwargs: drawn.append(kwargs.get("gradient")))
  renderer = _make_renderer(module, experimental_mode)

  renderer._update_experimental_gradient()
  renderer._draw_path()

  colors = renderer._exp_gradient.colors
  assert len(colors) > 1
  assert drawn == [renderer._exp_gradient]

  # Speeding up near the car reads green, slowing down further out reads red
  assert colors[0].g > colors[0].r
  assert colors[-1].r > colors[-1].g
