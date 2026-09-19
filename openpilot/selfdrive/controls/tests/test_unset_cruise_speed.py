"""An unset set speed must not reach the planner as a 145 kph cruise target."""

import unittest

from openpilot.cereal import messaging
from openpilot.common.constants import CV
from openpilot.selfdrive.car.cruise import V_CRUISE_UNSET
from openpilot.selfdrive.controls.lib.longcontrol import LongCtrlState
from openpilot.selfdrive.controls.lib.longitudinal_planner import LongitudinalPlanner
from opendbc.car.honda.interface import CarInterface
from opendbc.car.honda.values import CAR

NOW = 10_000_000_000


class Bus:
  def __init__(self, messages, stamp):
    self.messages = messages
    self.logMonoTime = dict.fromkeys(messages, stamp)
    self.valid = dict.fromkeys(messages, True)
    self.alive = dict.fromkeys(messages, True)

  def __getitem__(self, name):
    return self.messages[name]


def bus(stamp, v_ego, v_cruise_kph):
  names = ('modelV2', 'carState', 'carControl', 'controlsState', 'selfdriveState', 'radarState', 'vehicleParameters')
  events = {name: messaging.new_message(name) for name in names}
  events['modelV2'].modelV2.velocity.x = [v_ego] * 33
  events['modelV2'].modelV2.position.x = [float(i * 5) for i in range(33)]
  events['modelV2'].modelV2.position.y = [0.0] * 33
  events['carState'].carState.vCruise = v_cruise_kph
  events['carState'].carState.vEgo = v_ego
  events['carState'].carState.canValid = True
  events['carControl'].carControl.orientationNED = [0.0, 0.0, 0.0]
  events['controlsState'].controlsState.longControlState = LongCtrlState.off
  return Bus({name: getattr(messaging.log_from_bytes(msg.to_bytes()), name) for name, msg in events.items()}, stamp)


class UnsetCruiseSpeedTests(unittest.TestCase):
  def test_unset_cruise_speed_tracks_v_ego_not_the_placeholder(self):
    # An unset set speed reaches the planner as V_CRUISE_UNSET clamped to V_CRUISE_MAX. Planning
    # toward that placeholder while disengaged asks for full throttle until the driver first sets
    # a speed, and the step down to the real target lands as a brake on the first engage.
    v_ego = 27.0
    cp = CarInterface.get_non_essential_params(CAR.HONDA_CIVIC)
    planner = LongitudinalPlanner(cp, init_v=v_ego)
    for n in range(1, 21):
      planner.update(bus(NOW + n * 50_000_000, v_ego, V_CRUISE_UNSET))
    self.assertAlmostEqual(float(planner.a_cruise), 0.0, places=3)

  def test_a_set_cruise_speed_still_reaches_the_planner(self):
    v_ego = 27.0
    cp = CarInterface.get_non_essential_params(CAR.HONDA_CIVIC)
    planner = LongitudinalPlanner(cp, init_v=v_ego)
    for n in range(1, 21):
      planner.update(bus(NOW + n * 50_000_000, v_ego, 120.0))
    # disengaged, the cruise accel restarts from aEgo every cycle, but still points at the set speed
    self.assertGreater(120.0 * CV.KPH_TO_MS, v_ego)
    self.assertGreater(float(planner.a_cruise), 0.01)


if __name__ == "__main__":
  unittest.main()
