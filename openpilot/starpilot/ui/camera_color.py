"""High-contrast camera color: monochrome disengaged, muted for AOL, vivid Technicolor engaged."""

from openpilot.starpilot.ui.runtime_snapshot import display_message

ROAD_COLOR_SHADER = """
highp vec3 road_color(highp vec3 rgb, highp float strength) {
  const highp vec3 weights = vec3(0.299, 0.587, 0.114);
  // YUV conversion can produce negative channels; pow requires nonnegative input.
  rgb = pow(clamp(rgb, 0.0, 1.0), vec3(1.0 / 1.28));
  highp float gray = dot(rgb, weights);
  rgb = clamp((vec3(gray) + 1.45 * (rgb - vec3(gray)) - 0.5) * 1.25 + 0.5, 0.0, 1.0);
  return mix(vec3(dot(rgb, weights)), rgb, strength);
}
"""


def road_color_strength(ui) -> float:
  if not ui.started:
    return 0.0
  now = ui.display_time_ns()
  control = display_message(ui.sm, 'carControl', now, after_frame=ui.started_frame)
  if control is None:
    return 0.0
  selfdrive = display_message(ui.sm, 'selfdriveState', now, after_frame=ui.started_frame)
  # Enabled includes stock ACC and temporary pedal/steering overrides.
  if control.longActive or (selfdrive is not None and selfdrive.enabled):
    return 1.0
  return 0.45 if control.latActive else 0.0
