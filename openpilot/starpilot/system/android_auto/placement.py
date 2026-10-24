"""Keep projection work below ordinary UI work on core 6.

SCHED_BATCH with positive niceness permits progress when ordinary tasks keep
core 6 busy; SCHED_IDLE could spend almost an entire startup deadline runnable
but unscheduled. Real-time controls/model policies and CPU assignments remain
unchanged. Apply placement before importing the UI so driver threads inherit it,
and reapply affinity when the big cluster returns from offroad power saving.
"""

from __future__ import annotations

import os
from collections.abc import Callable
from typing import Any

RENDER_CORE = 6
PLACEMENT_CHECK_INTERVAL = 1.0  # s
RENDER_NICE = 5
FALLBACK_NICE = 10  # if SCHED_BATCH is unavailable


def _thread_ids() -> list[int]:
  try:
    return [int(tid) for tid in os.listdir("/proc/self/task")]
  except OSError:
    return [0]


def _core_online(core: int) -> bool:
  if core == 0:
    return True
  try:
    with open(f"/sys/devices/system/cpu/cpu{core}/online") as f:
      return f.read().strip() == "1"
  except OSError:
    return False


class RendererPlacement:
  """Keeps every thread of this process on RENDER_CORE while that core is online:
  Low-priority batch scheduling for the renderer, or normal scheduling (idle=False) for
  android_autod, whose stream thread otherwise sticks on whichever isolated
  core it first woke on (core 7, waiting ~32 % of the time behind the models)."""

  def __init__(self, core: int = RENDER_CORE, report: Callable[[dict[str, Any]], None] | None = None,
               thread_ids: Callable[[], list[int]] = _thread_ids, core_online: Callable[[int], bool] = _core_online,
               idle: bool = True):
    self.core = core
    self.idle = idle
    self.report = report or (lambda _event: None)
    self.thread_ids = thread_ids
    self.core_online = core_online
    self.next_check = 0.0
    self.pinned = False

  def start(self) -> None:
    """Call first, before any thread exists, so later threads inherit the policy."""
    if not hasattr(os, "sched_setaffinity"):
      self.report({"event": "cpu_placement", "pinned": False, "reason": "no CPU affinity on this platform"})
      self.next_check = float("inf")
      return
    inherited = sorted(os.sched_getaffinity(0))
    policy = "normal"
    try:
      if self.idle:
        os.sched_setscheduler(0, os.SCHED_BATCH, os.sched_param(0))
        os.nice(RENDER_NICE)
        policy = f"batch-nice{RENDER_NICE}"
    except (AttributeError, OSError):
      policy = f"nice{FALLBACK_NICE}"
      try:
        os.nice(FALLBACK_NICE)
      except OSError:
        policy = "default"
    self.report({"event": "cpu_placement", "policy": policy, "inherited_cores": inherited, "core": self.core})
    self.maintain(0.0)

  def maintain(self, now: float) -> None:
    if now < self.next_check:
      return
    self.next_check = now + PLACEMENT_CHECK_INTERVAL
    if not self.core_online(self.core):
      if self.pinned:
        self.report({"event": "cpu_placement", "pinned": False, "reason": f"core {self.core} offline"})
      self.pinned = False
      return

    target = {self.core}
    changed = False
    for tid in self.thread_ids():
      try:
        if os.sched_getaffinity(tid) != target:
          os.sched_setaffinity(tid, target)
          changed = True
      except OSError:
        pass  # the thread exited
    if changed or not self.pinned:
      self.report({"event": "cpu_placement", "pinned": True, "cores": sorted(os.sched_getaffinity(0))})
    self.pinned = True
