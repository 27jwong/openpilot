import numpy as np
import pytest

from opendbc.car.structs import car
from opendbc.car.mazda.values import MazdaSafetyFlags
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.drive_helpers import CONTROL_N, get_accel_from_plan
from openpilot.selfdrive.modeld.constants import ModelConstants
from openpilot.starpilot.longitudinal.learned_delay import MAX_LONG_DELAY, PARAM, REFRESH_NS, REGIME_BLEND, VERSION, LearnedDelayHost, \
                                                         brake_threshold, get_accel_from_plan_split, get_long_delays

V_EGO = 20.0
CONTROL_N_T_IDX = ModelConstants.T_IDXS[:CONTROL_N]


def gen2_CP(delay=0.35, fingerprint="MAZDA_CX_30"):
  return car.CarParams(brand="mazda", flags=MazdaSafetyFlags.GEN2.value, longitudinalActuatorDelay=delay, carFingerprint=fingerprint)


def saved(gas=0.25, brake=0.55, gas_status='estimated', brake_status='estimated', fingerprint="MAZDA_CX_30", version=VERSION):
  return {'version': version, 'carFingerprint': fingerprint,
          'gas': {'delay': gas, 'status': gas_status}, 'brake': {'delay': brake, 'status': brake_status}}


def plan_ramp(final_accel, ramp_start=0.2, ramp_time=0.8, v0=V_EGO):
  """A plan that coasts, then ramps into `final_accel`: what a planner sees ahead of a brake event."""
  t = np.array(CONTROL_N_T_IDX)
  accels = final_accel * np.clip((t - ramp_start) / ramp_time, 0.0, 1.0)
  speeds = v0 + np.concatenate(([0.0], np.cumsum((accels[1:] + accels[:-1]) / 2 * np.diff(t))))
  return speeds, accels


def sampler(speeds, accels):
  return lambda delay: (get_accel_from_plan(speeds, accels, CONTROL_N_T_IDX, action_t=delay + DT_MDL), False)


class FakeParams(dict):
  def get(self, key, block=False):
    self.reads = getattr(self, 'reads', 0) + 1
    return super().get(key)


class TestGetLongDelays:
  def test_unlearned_uses_car_delay(self):
    assert get_long_delays(gen2_CP()) == (pytest.approx(0.35), pytest.approx(0.35))
    assert get_long_delays(gen2_CP(), saved(gas_status='unestimated')) == (pytest.approx(0.35),) * 2

  def test_learned_split_on_gen2(self):
    assert get_long_delays(gen2_CP(), saved()) == (pytest.approx(0.25), pytest.approx(0.55))

  def test_other_platforms_stay_in_shadow_mode(self):
    CP = car.CarParams(brand="toyota", longitudinalActuatorDelay=0.4, carFingerprint="MAZDA_CX_30")
    assert get_long_delays(CP, saved()) == (pytest.approx(0.4), pytest.approx(0.4))

  @pytest.mark.parametrize("state", [saved(fingerprint="MAZDA_3_2019"), saved(version=VERSION + 1), {'gas': 1}, "garbage",
                                     saved(gas=float('nan'))])
  def test_unusable_saves_are_ignored(self, state):
    assert get_long_delays(gen2_CP(), state) == (pytest.approx(0.35),) * 2

  def test_delays_are_bounded(self):
    assert get_long_delays(gen2_CP(), saved(0.0, 3.0)) == (pytest.approx(0.05), pytest.approx(MAX_LONG_DELAY))


class TestSplitSampling:
  @pytest.mark.parametrize("accel", [-2.0, -0.4, 0.0, 0.8])
  def test_equal_delays_match_single_delay(self, accel):
    speeds, accels = plan_ramp(accel)
    assert get_accel_from_plan_split(sampler(speeds, accels), V_EGO, 0.35, 0.35) == sampler(speeds, accels)(0.35)

  def test_braking_uses_brake_lookahead(self):
    # the brake lookahead only sees about a third of the ramp's final accel, so this has to be deep
    # enough that it lands past the brake threshold (-0.75 at this speed) plus half the crossfade
    speeds, accels = plan_ramp(-3.0)
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

  def test_should_stop_follows_the_regime(self):
    # a sampler whose stop decision says which lookahead it came from
    def stops_on_brake_lookahead(final_accel):
      speeds, accels = plan_ramp(final_accel)
      return lambda delay: (sampler(speeds, accels)(delay)[0], delay > 0.4)
    assert get_accel_from_plan_split(stops_on_brake_lookahead(-3.0), V_EGO, 0.25, 0.55)[1] is True
    assert get_accel_from_plan_split(stops_on_brake_lookahead(1.0), V_EGO, 0.25, 0.55)[1] is False

  def test_no_step_across_threshold(self):
    thr = brake_threshold(V_EGO)
    # plan_ramp(f) only reaches a fraction of f at the brake lookahead, and the crossfade is decided
    # there; scale the sweep so the request it sees crosses the whole blend, not just its edge
    seen_per_final = abs(sampler(*plan_ramp(-1.0))(0.55)[0])
    finals = np.linspace(thr - 3 * REGIME_BLEND, thr + 3 * REGIME_BLEND, 400) / seen_per_final
    targets = [get_accel_from_plan_split(sampler(*plan_ramp(f)), V_EGO, 0.25, 0.55)[0] for f in finals]
    steps = np.abs(np.diff(targets))
    # a smooth crossfade moves no faster than the plan itself does, give or take the blend's slope
    assert steps.max() < 3 * np.diff(finals).max()


class TestLearnedDelayHost:
  def test_rereads_only_every_refresh(self):
    params = FakeParams({PARAM: saved()})
    host = LearnedDelayHost(params, gen2_CP())
    assert host.refresh(0) == (pytest.approx(0.25), pytest.approx(0.55))

    params[PARAM] = saved(0.3, 0.6)
    assert host.refresh(REFRESH_NS - 1) == (pytest.approx(0.25), pytest.approx(0.55))
    assert host.refresh(REFRESH_NS) == (pytest.approx(0.3), pytest.approx(0.6))
    assert params.reads == 2

  def test_missing_save_keeps_car_delay(self):
    assert LearnedDelayHost(FakeParams(), gen2_CP()).refresh(0) == (pytest.approx(0.35),) * 2

  def test_shadow_platforms_never_read(self):
    params = FakeParams({PARAM: saved()})
    CP = car.CarParams(brand="toyota", longitudinalActuatorDelay=0.4)
    assert LearnedDelayHost(params, CP).refresh(0) == (pytest.approx(0.4),) * 2
    assert getattr(params, 'reads', 0) == 0
