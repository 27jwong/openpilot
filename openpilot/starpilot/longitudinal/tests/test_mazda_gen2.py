"""Mazda GEN2 longitudinal tune, end to end through LongControl."""
from types import SimpleNamespace
import unittest

from opendbc.car import structs
from opendbc.car.mazda.longitudinal import MazdaGen2LongitudinalPolicy
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.controls.lib.longcontrol import LongControl, LongCtrlState
from openpilot.starpilot.longitudinal.extension import LongitudinalContext


def make_mazda_gen2_cp(flags=MazdaSafetyFlags.GEN2):
  cp = structs.CarParams()
  cp.brand = "mazda"
  cp.flags = int(flags)
  cp.openpilotLongitudinalControl = True
  cp.stopAccel = -0.5
  cp.longitudinalTuning.deprecated.kpBP = [0.0, 12.0, 30.0]
  cp.longitudinalTuning.deprecated.kpV = [0.2, 0.5, 0.4]
  cp.longitudinalTuning.kiBP = [0.0, 35.0]
  cp.longitudinalTuning.kiV = [1.0, 1.0]
  return cp


def car_state(v_ego, a_ego=0.0):
  return SimpleNamespace(vEgo=v_ego, aEgo=a_ego, brakePressed=False, gasPressed=False, canValid=True,
                         canTimeout=False, cruiseState=SimpleNamespace(standstill=False))


class TestMazdaGen2LongControl(unittest.TestCase):
  def test_policy_is_selected_for_gen2_only(self):
    self.assertIsInstance(LongControl(make_mazda_gen2_cp()).extension.vehicle_policy, MazdaGen2LongitudinalPolicy)
    self.assertIsNone(LongControl(make_mazda_gen2_cp(MazdaSafetyFlags.GEN1)).extension)

  def test_integrator_corrects_a_standing_offset(self):
    """The old kp=0/ki=0.1 tune needed ~10s to cancel a steady offset. Check the new
    gains close most of a held error within a couple of seconds."""
    lc = LongControl(make_mazda_gen2_cp())
    output = 0.0
    for _ in range(200):  # 2 s at 100 Hz, car stubbornly not responding
      output = lc.update(True, car_state(20.0), 0.5, False, (-3.5, 2.0), context=LongitudinalContext())
    self.assertEqual(lc.long_control_state, LongCtrlState.pid)
    # feedforward alone would sit at a_target; the loop must be commanding meaningfully more
    self.assertGreater(output, 0.5 + 0.5)
    self.assertGreater(lc.pid.i, 0.5)

  def test_emergency_braking_authority_is_preserved(self):
    """At the accel floor the cap must not reduce available braking."""
    lc = LongControl(make_mazda_gen2_cp())
    output = lc.update(True, car_state(25.0, -1.0), -3.5, False, (-3.5, 2.0), context=LongitudinalContext())
    self.assertAlmostEqual(output, -3.5, places=6)

  def test_pitch_feedforward_reaches_the_command(self):
    def settled_on_the_flat():
      lc = LongControl(make_mazda_gen2_cp())
      for _ in range(int(15.0 / DT_CTRL)):
        lc.update(True, car_state(25.0), 0.0, False, (-3.5, 2.0), context=LongitudinalContext(pitch=0.0))
      return lc

    climbing, level = settled_on_the_flat(), settled_on_the_flat()
    self.assertAlmostEqual(level.update(True, car_state(25.0), 0.0, False, (-3.5, 2.0),
                                        context=LongitudinalContext(pitch=0.0)), 0.0, places=6)
    self.assertAlmostEqual(climbing.update(True, car_state(25.0), 0.0, False, (-3.5, 2.0),
                                           context=LongitudinalContext(pitch=0.04)), 0.392, delta=0.01)

  def test_pitch_feedforward_defaults_to_off_without_a_pose(self):
    """calibrated_pose is None before locationd converges; the loop must behave as it does without it."""
    lc = LongControl(make_mazda_gen2_cp())
    self.assertEqual(lc.update(True, car_state(25.0), 0.0, False, (-3.5, 2.0), context=LongitudinalContext()), 0.0)


if __name__ == "__main__":
  unittest.main()
