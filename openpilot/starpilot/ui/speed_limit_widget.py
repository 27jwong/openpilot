"""One speed-limit sign, attention animation and override mark for both screens."""

import math
import time

import pyray as rl

from openpilot.starpilot.ui.onroad_state import OnroadState, speed_limit_bounds
from openpilot.starpilot.ui.presentation import BitmapFonts, FontRole, Profile
from openpilot.starpilot.ui.speed_limit_pulse import SpeedLimitPulse
from openpilot.starpilot.ui.unified_speed_presentation import displayed_adjusted_limit_mps, resolve_unified_speed


class SpeedLimitWidget:
  def __init__(self, fonts: BitmapFonts, profile: Profile):
    self.fonts = fonts
    self.profile = profile
    self.pulse = SpeedLimitPulse()

  def render(self, state: OnroadState) -> None:
    bounds = speed_limit_bounds(self.profile, state)
    now = state.observed_ns / 1e9 if state.observed_ns else time.monotonic()
    self.pulse.update(state, now, visible=bounds is not None)
    if bounds is None:
      return
    shown, obs = resolve_unified_speed(state), state.speed_limit
    pending = shown.pending and not obs.automatic_adoption_pending
    elapsed = max(0., now - obs.pending_since_ns / 1e9)
    hop = -6 * math.sin(math.pi * (elapsed % 1.5) / 1.5) ** 2 if pending else 0
    left, top, right, bottom = bounds
    rect = rl.Rectangle(left, top + hop, right - left, bottom - top)
    white = self.pulse.color(rl.WHITE, now)

    def label(text, size, y, color=white, role=FontRole.SEMI_BOLD):
      measured = self.fonts.measure(text, role, size)
      self.fonts.draw(text, role, size, rect.x + (rect.width - measured.width) / 2, rect.y + y, color)

    if state.appearance.use_vienna_sign:
      center = rl.Vector2(rect.x + rect.width / 2, rect.y + 58)
      rl.draw_circle(int(center.x), int(center.y), 54, rl.WHITE)
      rl.draw_ring(center, 44, 54, 0, 360, 64, self.pulse.color(rl.Color(201, 34, 49, 255), now))
      label(shown.posted_text, 50 if len(shown.posted_text) <= 2 else 42, 28, self.pulse.color(rl.BLACK, now), FontRole.BOLD)
    else:
      rl.draw_rectangle_rounded_lines_ex(rl.Rectangle(rect.x + 4, rect.y + 4, rect.width - 8, rect.height - 8), .14, 16, 2, white)
      label('SPEED', 17, 12)
      label('LIMIT', 17, 30)
      label(shown.posted_text, 50 if len(shown.posted_text) <= 2 else 42, 48, role=FontRole.BOLD)
    if shown.offset_text:
      label(shown.offset_text, 22, 105)
    if pending:
      target = displayed_adjusted_limit_mps(obs)
      if target is not None and state.speed_mps is not None and target != state.speed_mps:
        x, y = rect.x + rect.width - 12, rect.y + 25
        direction = -1 if target > state.speed_mps else 1
        tip = rl.Vector2(x, y + direction * 8)
        rl.draw_line_ex(rl.Vector2(x, y - direction * 8), tip, 2, white)
        for dx in (-5, 5):
          rl.draw_line_ex(tip, rl.Vector2(x + dx, tip.y - direction * 6), 2, white)
    if obs.driver_override_active:
      color = rl.Color(197, 163, 255, 230)
      rl.draw_line_ex(rl.Vector2(rect.x + 8, rect.y + 8), rl.Vector2(rect.x + rect.width - 8, rect.y + rect.height - 8), 4, color)
      rl.draw_line_ex(rl.Vector2(rect.x + rect.width - 8, rect.y + 8), rl.Vector2(rect.x + 8, rect.y + rect.height - 8), 4, color)
