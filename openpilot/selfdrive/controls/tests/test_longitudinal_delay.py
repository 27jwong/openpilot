import unittest

import numpy as np

from openpilot.cereal import messaging
from opendbc.car.structs import car
from opendbc.car.mazda.interface import CarInterface as MazdaCarInterface
from opendbc.car.mazda.values import CAR as MAZDA_CAR, MazdaSafetyFlags
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.drive_helpers import CONTROL_N
from openpilot.selfdrive.controls.lib.longitudinal_planner import CONTROL_N_T_IDX, LongitudinalPlanner, get_accel_from_plan, \
                                                                  get_accel_from_plan_split
from openpilot.selfdrive.controls.tests.test_unset_cruise_speed import NOW, bus
from openpilot.starpilot.longitudinal.longitudinal_delay import MAX_LONG_DELAY, MIN_LONG_DELAY, REGIME_BLEND, brake_threshold, \
                                                                get_long_delays

V_EGO = 20.0


def gen2_CP(delay=0.35):
  return car.CarParams.new_message(brand="mazda", flags=MazdaSafetyFlags.GEN2.value, longitudinalActuatorDelay=delay)


def live_delay(gas=0.25, brake=0.55, gas_status='estimated', brake_status='estimated'):
  msg = messaging.new_message('starpilotLongitudinalDelay').starpilotLongitudinalDelay
  msg.gas.delay, msg.gas.status = gas, gas_status
  msg.brake.delay, msg.brake.status = brake, brake_status
  return msg


def plan_ramp(final_accel, ramp_start=0.2, ramp_time=0.8, v0=V_EGO):
  """A plan that coasts, then ramps into `final_accel`: what a planner sees ahead of a brake event."""
  t = np.array(CONTROL_N_T_IDX)
  accels = final_accel * np.clip((t - ramp_start) / ramp_time, 0.0, 1.0)
  speeds = v0 + np.concatenate(([0.0], np.cumsum((accels[1:] + accels[:-1]) / 2 * np.diff(t))))
  assert len(speeds) == CONTROL_N
  return speeds, accels


def sample(speeds, accels, delay):
  return get_accel_from_plan(speeds, accels, CONTROL_N_T_IDX, action_t=delay + DT_MDL)


def sampler(speeds, accels):
  return lambda delay: (sample(speeds, accels, delay), False)


class TestGetLongDelays(unittest.TestCase):
  def assertDelays(self, delays, expected):
    self.assertEqual(len(delays), 2)
    for got, want in zip(delays, expected, strict=True):
      self.assertAlmostEqual(got, want, places=5)

  def test_unlearned_uses_car_delay(self):
    self.assertDelays(get_long_delays(gen2_CP()), (0.35, 0.35))
    self.assertDelays(get_long_delays(gen2_CP(), live_delay(gas_status='unestimated')), (0.35, 0.35))
    self.assertDelays(get_long_delays(gen2_CP(), live_delay(brake_status='invalid')), (0.35, 0.35))

  def test_learned_split_on_gen2(self):
    self.assertDelays(get_long_delays(gen2_CP(), live_delay()), (0.25, 0.55))

  def test_other_platforms_stay_in_shadow_mode(self):
    CP = car.CarParams.new_message(brand="toyota", longitudinalActuatorDelay=0.4)
    self.assertDelays(get_long_delays(CP, live_delay()), (0.4, 0.4))
    gen1 = car.CarParams.new_message(brand="mazda", flags=MazdaSafetyFlags.GEN1.value, longitudinalActuatorDelay=0.4)
    self.assertDelays(get_long_delays(gen1, live_delay()), (0.4, 0.4))

  def test_delays_are_bounded(self):
    self.assertDelays(get_long_delays(gen2_CP(), live_delay(0.0, 3.0)), (MIN_LONG_DELAY, MAX_LONG_DELAY))


class TestSplitSampling(unittest.TestCase):
  def test_equal_delays_match_single_delay(self):
    for accel in (-2.0, -0.4, 0.0, 0.8):
      with self.subTest(accel=accel):
        speeds, accels = plan_ramp(accel)
        a_target, _ = get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.35, 0.35)
        self.assertEqual(a_target, sample(speeds, accels, 0.35))

  def test_braking_uses_brake_lookahead(self):
    speeds, accels = plan_ramp(-2.0)
    a_target, _ = get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.25, 0.55)
    self.assertAlmostEqual(a_target, sample(speeds, accels, 0.55), places=6)
    # the brake request comes earlier and harder than the gas lookahead would have asked for
    self.assertLess(a_target, sample(speeds, accels, 0.25))

  def test_accelerating_uses_gas_lookahead(self):
    speeds, accels = plan_ramp(1.0)
    a_target, _ = get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.25, 0.55)
    self.assertAlmostEqual(a_target, sample(speeds, accels, 0.25), places=6)

  def test_should_stop_follows_the_dominant_regime(self):
    speeds, accels = plan_ramp(-2.0)
    split = get_accel_from_plan_split(lambda delay: (sample(speeds, accels, delay), delay > 0.5), V_EGO, 0.25, 0.55)
    self.assertTrue(split[1])
    speeds, accels = plan_ramp(1.0)
    split = get_accel_from_plan_split(lambda delay: (sample(speeds, accels, delay), delay > 0.5), V_EGO, 0.25, 0.55)
    self.assertFalse(split[1])

  def test_no_step_across_threshold(self):
    thr = brake_threshold(V_EGO)
    finals = np.linspace(thr - 3 * REGIME_BLEND, thr + 3 * REGIME_BLEND, 400)
    targets = [get_accel_from_plan_split(sampler(*plan_ramp(f)), V_EGO, 0.25, 0.55)[0] for f in finals]
    steps = np.abs(np.diff(targets))
    # a smooth crossfade moves no faster than the plan itself does, give or take the blend's slope
    self.assertLess(steps.max(), 3 * np.diff(finals).max())


def delay_bus(stamp, live=None):
  frame = bus(stamp, V_EGO, 120.0)
  frame.seen = {'starpilotLongitudinalDelay': live is not None}
  if live is not None:
    frame.messages['starpilotLongitudinalDelay'] = live
  return frame


class TestPlannerIntegration(unittest.TestCase):
  def test_planner_picks_up_learned_delays(self):
    CP = MazdaCarInterface.get_non_essential_params(MAZDA_CAR.MAZDA_CX_30)
    planner = LongitudinalPlanner(CP, init_v=V_EGO)
    planner.update(delay_bus(NOW, live_delay(0.25, 0.55)))
    self.assertAlmostEqual(planner.gas_delay, 0.25, places=5)
    self.assertAlmostEqual(planner.brake_delay, 0.55, places=5)
    self.assertAlmostEqual(planner.lead_approach.actuator_delay, 0.55, places=5)

  def test_planner_without_learner_keeps_car_delay(self):
    CP = MazdaCarInterface.get_non_essential_params(MAZDA_CAR.MAZDA_CX_30)
    planner = LongitudinalPlanner(CP, init_v=V_EGO)
    planner.update(delay_bus(NOW))
    self.assertAlmostEqual(planner.gas_delay, CP.longitudinalActuatorDelay, places=5)
    self.assertAlmostEqual(planner.brake_delay, CP.longitudinalActuatorDelay, places=5)


if __name__ == "__main__":
  unittest.main()
