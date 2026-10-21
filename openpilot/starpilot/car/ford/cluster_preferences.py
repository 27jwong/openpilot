"""Dom-compatible opt-in cluster display, independent of lateral authority."""
from openpilot.starpilot.saved_source import read_saved

KEY = "FordHandsFreeCluster"
CHOICES = {0: "Off", 1: "On"}


def read_choice(params):
  raw, readable = read_saved(params, KEY, 8)
  valid = readable and raw in (None, b"0", b"1")
  return int(valid and raw == b"1"), raw, readable, valid


class MachEClusterPreference:
  def __init__(self, params):
    self.params = params
    self.enabled = bool(read_choice(params)[0])
    self.frames = 0

  def update(self):
    if self.frames % 100 == 0:
      self.enabled = bool(read_choice(self.params)[0])
    self.frames += 1
