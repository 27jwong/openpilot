from types import SimpleNamespace

import numpy as np
import pytest

from cereal import car, messaging
from opendbc.car.mazda.interface import CarInterface as MazdaCarInterface
from opendbc.car.mazda.values import CAR as MAZDA_CAR, MazdaSafetyFlags
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.drive_helpers import CONTROL_N
from openpilot.selfdrive.controls.lib.longitudinal_planner import CONTROL_N_T_IDX, LongitudinalPlanner, get_accel_from_plan, \
                                                                  get_accel_from_plan_split
from openpilot.selfdrive.controls.tests.test_longitudinal_planner import make_sm, make_toggles
from openpilot.starpilot.common.longitudinal_delay import MAX_LONG_DELAY, REGIME_BLEND, brake_threshold, get_long_delays

V_EGO = 20.0


def gen2_CP(delay=0.35):
  return car.CarParams(brand="mazda", flags=MazdaSafetyFlags.GEN2.value, longitudinalActuatorDelay=delay)


def live_delay(gas=0.25, brake=0.55, gas_status='estimated', brake_status='estimated'):
  msg = messaging.new_message('starpilotLongitudinalDelay').starpilotLongitudinalDelay
  msg.gas.delay, msg.gas.status = gas, gas_status
  msg.brake.delay, msg.brake.status = brake, brake_status
  return msg


def plan_with_accel(accel, v0=V_EGO):
  """A plan that holds `accel` from t=0, so the lookahead decides how much of it is seen."""
  t = np.array(CONTROL_N_T_IDX)
  speeds = np.maximum(v0 + accel * t, 0.0)
  accels = np.full(CONTROL_N, accel)
  return speeds, accels


def plan_ramp(final_accel, ramp_start=0.2, ramp_time=0.8, v0=V_EGO):
  """A plan that coasts, then ramps into `final_accel`: what a planner sees ahead of a brake event."""
  t = np.array(CONTROL_N_T_IDX)
  accels = final_accel * np.clip((t - ramp_start) / ramp_time, 0.0, 1.0)
  speeds = v0 + np.concatenate(([0.0], np.cumsum((accels[1:] + accels[:-1]) / 2 * np.diff(t))))
  return speeds, accels


def sampler(speeds, accels):
  return lambda delay: get_accel_from_plan(speeds, accels, action_t=delay + DT_MDL, vEgoStopping=0.5)


class TestGetLongDelays:
  def test_unlearned_uses_car_delay(self):
    assert get_long_delays(gen2_CP(), SimpleNamespace()) == (pytest.approx(0.35), pytest.approx(0.35))
    assert get_long_delays(gen2_CP(), SimpleNamespace(), live_delay(gas_status='unestimated')) == (pytest.approx(0.35),) * 2

  def test_learned_split_on_gen2(self):
    assert get_long_delays(gen2_CP(), SimpleNamespace(), live_delay()) == (pytest.approx(0.25), pytest.approx(0.55))

  def test_other_platforms_stay_in_shadow_mode(self):
    CP = car.CarParams(brand="toyota", longitudinalActuatorDelay=0.4)
    assert get_long_delays(CP, SimpleNamespace(), live_delay()) == (pytest.approx(0.4), pytest.approx(0.4))

  def test_slider_sets_base_delay(self):
    toggles = SimpleNamespace(longitudinalActuatorDelay=0.5, use_custom_longitudinalActuatorDelay=True)
    assert get_long_delays(gen2_CP(), toggles) == (pytest.approx(0.5), pytest.approx(0.5))

  def test_slider_recentres_learned_split(self):
    toggles = SimpleNamespace(longitudinalActuatorDelay=0.5, use_custom_longitudinalActuatorDelay=True)
    gas, brake = get_long_delays(gen2_CP(), toggles, live_delay(0.25, 0.55))
    assert gas == pytest.approx(0.35)
    assert brake == pytest.approx(0.65)

  def test_delays_are_bounded(self):
    assert get_long_delays(gen2_CP(), SimpleNamespace(), live_delay(0.0, 3.0)) == (pytest.approx(0.05), pytest.approx(MAX_LONG_DELAY))
    assert get_long_delays(gen2_CP(), SimpleNamespace(longitudinalActuatorDelay=0.0)) == (pytest.approx(0.35),) * 2


class TestSplitSampling:
  @pytest.mark.parametrize("accel", [-2.0, -0.4, 0.0, 0.8])
  def test_equal_delays_match_single_delay(self, accel):
    speeds, accels = plan_ramp(accel)
    expected = get_accel_from_plan(speeds, accels, action_t=0.35 + DT_MDL, vEgoStopping=0.5)
    assert get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.35, 0.35) == expected

  def test_braking_uses_brake_lookahead(self):
    speeds, accels = plan_ramp(-2.0)
    a_target, _ = get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.25, 0.55)
    a_brake, _ = sampler(speeds, accels)(0.55)
    a_gas, _ = sampler(speeds, accels)(0.25)
    assert a_target == pytest.approx(a_brake)
    # the brake request comes earlier and harder than the gas lookahead would have asked for
    assert a_target < a_gas

  def test_accelerating_uses_gas_lookahead(self):
    speeds, accels = plan_ramp(1.0)
    a_target, _ = get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.25, 0.55)
    assert a_target == pytest.approx(sampler(speeds, accels)(0.25)[0])

  def test_no_step_across_threshold(self):
    thr = brake_threshold(V_EGO)
    finals = np.linspace(thr - 3 * REGIME_BLEND, thr + 3 * REGIME_BLEND, 400)
    targets = [get_accel_from_plan_split(sampler(*plan_ramp(f)), V_EGO, 0.25, 0.55)[0] for f in finals]
    steps = np.abs(np.diff(targets))
    # a smooth crossfade moves no faster than the plan itself does, give or take the blend's slope
    assert steps.max() < 3 * np.diff(finals).max()


class SubMasterLike(dict):
  def __init__(self, data, seen):
    super().__init__(data)
    self.seen = seen


class TestPlannerIntegration:
  def test_planner_picks_up_learned_delays(self):
    CP = MazdaCarInterface.get_non_essential_params(MAZDA_CAR.MAZDA_CX_30)
    planner = LongitudinalPlanner(CP, init_v=V_EGO)
    sm = make_sm(V_EGO, -0.5, -3.5)
    sm = SubMasterLike({**sm, 'starpilotLongitudinalDelay': live_delay(0.25, 0.55)}, seen={'starpilotLongitudinalDelay': True})
    planner.update(sm, make_toggles())
    assert (planner.gas_delay, planner.brake_delay) == (pytest.approx(0.25), pytest.approx(0.55))

  def test_planner_without_learner_keeps_car_delay(self):
    CP = MazdaCarInterface.get_non_essential_params(MAZDA_CAR.MAZDA_CX_30)
    planner = LongitudinalPlanner(CP, init_v=V_EGO)
    planner.update(make_sm(V_EGO, -0.5, -3.5), make_toggles())
    assert planner.gas_delay == planner.brake_delay == pytest.approx(CP.longitudinalActuatorDelay)
