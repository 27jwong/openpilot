import pytest

from opendbc.car.car_helpers import interfaces
from opendbc.car.structs import CarParams
from openpilot.starpilot.conditional_mode.stop_release import owner_for


BRANDS = sorted({interface.__module__.split('.')[-2] for interface in interfaces.values()})


@pytest.mark.parametrize('brand', BRANDS)
def test_cem_departure_owner_is_shared_across_active_longitudinal_configs(brand):
  for identity, interface in interfaces.items():
    if interface.__module__.split('.')[-2] != brand:
      continue
    cp = interface.get_non_essential_params(identity)
    cp.openpilotLongitudinalControl = True
    assert (owner_for(cp) is not None) == (not (cp.passive or cp.dashcamOnly or cp.notCar)), identity


@pytest.mark.parametrize('flag', ['passive', 'dashcamOnly', 'notCar'])
def test_inactive_profiles_never_get_departure_owner(flag):
  cp = CarParams(openpilotLongitudinalControl=True)
  setattr(cp, flag, True)
  assert owner_for(cp) is None


def test_stock_longitudinal_never_gets_departure_owner():
  assert owner_for(CarParams(openpilotLongitudinalControl=False)) is None


from types import SimpleNamespace
from functools import lru_cache
from openpilot.cereal import messaging
from openpilot.selfdrive.controls.radard import get_RadarState_from_vision
from openpilot.starpilot.conditional_mode.stop import STOP_MODEL_RELEASE_MARGIN_M
from openpilot.starpilot.conditional_mode.stop_release import LEAD_MIN_GAP_M, LEAD_PASS_MARGIN_M, StoppedLeadRelease
from openpilot.starpilot.longitudinal.tests import test_cem_green_release as green
from openpilot.starpilot.tests.test_ford_recorded_departure import _motion_payloads, _samples


@lru_cache(maxsize=1)
def recorded_motion():
  return next(sample for sample in _samples() if 334.44 < sample['routeSeconds'] < 334.48)


def scene(now, vision=False):
  motion = recorded_motion()
  payloads = _motion_payloads(green.serialized_scene(stamp=green.BOOT + now - green.MONO - 2_000_000,
    speed=0., standstill=True), motion)
  for service in ('carControl', 'controlsState'):
    event = messaging.new_message(service)
    setattr(event, service, payloads[service])
    payloads[service] = getattr(event, service)
  if vision:
    model = payloads['modelV2']
    for slot, lead in enumerate((payloads['radarState'].leadOne, payloads['radarState'].leadTwo)):
      if lead.present:
        raw = model.leadsV3[slot]
        lead.from_dict(get_RadarState_from_vision(raw, payloads['carState'].vEgo, model.velocity.x[0], raw.prob))
  return green.CallerFrame(payloads, now - 2_000_000)


@pytest.mark.parametrize('brand', BRANDS)
@pytest.mark.parametrize('vision', [False, True])
def test_shared_release_preserves_native_target_with_radar_or_correlated_vision(brand, vision):
  cp = next(interface.get_non_essential_params(identity) for identity, interface in interfaces.items()
            if interface.__module__.split('.')[-2] == brand)
  cp.openpilotLongitudinalControl = True
  owner = owner_for(cp)
  if cp.passive or cp.dashcamOnly or cp.notCar:
    assert owner is None
    return
  assert owner is not None
  detector = SimpleNamespace(committed=True, standstill_committed=True, standstill_reason='light', commit_distance_m=3.)
  planner = SimpleNamespace(mpc=SimpleNamespace(solution_status=0), output_should_stop=False, output_a_target=.5,
                            force_stop_plan=SimpleNamespace(forcing=False, should_stop=False))
  results = []
  for tick in range(13):
    now = green.MONO + 1_000_000_000 + tick * 50_000_000
    results.append(owner.sample(scene(now, vision), cp, planner, detector, now_ns=now, drive_id=green.MONO-1_000_000_000))
    assert planner.output_a_target == .5 and planner.output_should_stop is False
  assert not any(results[:10]) and all(results[10:])


@pytest.mark.parametrize('defect', ['mismatched-model', 'weak-model-lead', 'native-stop', 'native-braking', 'force-stop',
                                  'second-stopped', 'model-stop', 'stale', 'sign', 'config-change', 'source-change'])
def test_vision_release_requires_continuous_corroboration_and_native_go(defect):
  cp = CarParams(openpilotLongitudinalControl=True, carFingerprint='KIA_NIRO_EV')
  owner = StoppedLeadRelease()
  detector = SimpleNamespace(committed=True, standstill_committed=True, standstill_reason='light', commit_distance_m=3.)
  planner = SimpleNamespace(mpc=SimpleNamespace(solution_status=0), output_should_stop=False, output_a_target=.5,
                            force_stop_plan=SimpleNamespace(forcing=False, should_stop=False))
  results = []
  for tick in range(13):
    now = green.MONO + 1_000_000_000 + tick * 50_000_000
    sm = scene(now, True)
    if defect == 'mismatched-model':
      sm['radarState'].leadOne.dRel += 1.
    elif defect == 'weak-model-lead':
      sm['modelV2'].leadsV3[0].prob = .5
    elif defect == 'native-stop':
      planner.output_should_stop = True
    elif defect == 'native-braking':
      planner.output_a_target = -.1
    elif defect == 'force-stop':
      planner.force_stop_plan.forcing = True
    elif defect == 'second-stopped':
      sm['radarState'].leadTwo.vLead = 0.
    elif defect == 'model-stop':
      sm['modelV2'].action.shouldStop = True
    elif defect == 'stale':
      sm.logMonoTime['radarState'] = now - 150_000_001
    elif defect == 'sign':
      detector.standstill_reason = 'sign'
    elif defect == 'config-change':
      cp.carFingerprint = str(tick)
    elif defect == 'source-change' and tick % 2:
      sm = scene(now, False)
    results.append(owner.sample(sm, cp, planner, detector, now_ns=now, drive_id=green.MONO-1_000_000_000))
  assert not any(results)


def _lead_following_path(sm, extent):
  # A model following a departing lead plans well short of the raw 50 m stop distance.
  model = sm['modelV2']
  distances = list(model.position.x)
  model.position.x = [x * extent / max(distances) for x in distances]


def _release_ticks(extent_from_lead, commit_distance_m=3., lead_gap_m=None):
  cp = CarParams(openpilotLongitudinalControl=True, carFingerprint='MAZDA_CX30')
  owner = StoppedLeadRelease()
  detector = SimpleNamespace(committed=True, standstill_committed=True, standstill_reason='light', commit_distance_m=commit_distance_m)
  planner = SimpleNamespace(mpc=SimpleNamespace(solution_status=0), output_should_stop=False, output_a_target=.5,
                            force_stop_plan=SimpleNamespace(forcing=False, should_stop=False))
  results = []
  for tick in range(13):
    now = green.MONO + 1_000_000_000 + tick * 50_000_000
    sm = scene(now, lead_gap_m is None)
    if lead_gap_m is not None:
      # radar leads, so the distance can move without the vision correlation objecting
      for lead in (sm['radarState'].leadOne, sm['radarState'].leadTwo):
        if lead.present:
          lead.dRel = lead_gap_m
    lead_m = max(lead.dRel for lead in (sm['radarState'].leadOne, sm['radarState'].leadTwo) if lead.present)
    _lead_following_path(sm, extent_from_lead(lead_m))
    results.append(owner.sample(sm, cp, planner, detector, now_ns=now, drive_id=green.MONO-1_000_000_000))
  return results


def test_lead_following_path_short_of_raw_stop_distance_releases():
  # CX-30 2026-10-31: lead 6-10 m away at 0.6-1.8 m/s, model path 42-48 m; held until the driver pressed gas.
  results = _release_ticks(lambda lead_m: 45.)
  assert not any(results[:10]) and all(results[10:])


def test_path_that_does_not_clear_the_lead_never_releases():
  assert not any(_release_ticks(lambda lead_m: lead_m + LEAD_PASS_MARGIN_M + STOP_MODEL_RELEASE_MARGIN_M - .5))


def test_lead_departing_from_a_close_stop_releases():
  # CX-30 2026-11-11: stopped 4.1 m behind the lead, which crept to 5.5 m and later left from there.
  # It was doing 0.6-1.3 m/s with a 38-44 m model path for 1.1 s before it reached upstream's 6.3 m.
  results = _release_ticks(lambda lead_m: 40., lead_gap_m=5.7)
  assert not any(results[:10]) and all(results[10:])


def test_lead_inside_the_minimum_gap_never_releases():
  assert not any(_release_ticks(lambda lead_m: 40., lead_gap_m=LEAD_MIN_GAP_M - .1))


def test_committed_stop_beyond_the_lead_still_holds():
  assert not any(_release_ticks(lambda lead_m: 45., commit_distance_m=60.))


@pytest.mark.parametrize('vision', [False, True])
def test_niro_actual_cem_hold_releases_only_after_native_go(monkeypatch, vision):
  from opendbc.car.hyundai.values import CAR
  from openpilot.selfdrive.controls.lib.longitudinal_planner import LongitudinalPlanner
  case = green.CemGreenCallerTests(methodName='runTest')
  case.setUp()
  case.cp = interfaces[CAR.KIA_NIRO_EV].get_non_essential_params(CAR.KIA_NIRO_EV)
  case.cp.openpilotLongitudinalControl = True
  case.cp.pcmCruise = False
  case.planner = LongitudinalPlanner(case.cp, init_v=4.27)
  try:
    case.prime()
    original = green.serialized_scene
    samples = [sample for sample in _samples() if 334.29 <= sample['routeSeconds'] <= 335.3]
    motion = samples[0]
    def moving_lead(**kwargs):
      payloads = _motion_payloads(original(**kwargs), motion)
      if vision:
        model = payloads['modelV2']
        for slot, lead in enumerate((payloads['radarState'].leadOne, payloads['radarState'].leadTwo)):
          if lead.present:
            raw = model.leadsV3[slot]
            lead.from_dict(get_RadarState_from_vision(raw, payloads['carState'].vEgo, model.velocity.x[0], raw.prob))
      return payloads
    monkeypatch.setattr(green, 'serialized_scene', moving_lead)
    observations = []
    original_sample = StoppedLeadRelease.sample
    def observed(owner, sm, cp, planner, detector, **kwargs):
      before = float(planner.output_a_target), bool(planner.output_should_stop)
      released = original_sample(owner, sm, cp, planner, detector, **kwargs)
      observations.append((released, before, float(planner.output_a_target), bool(planner.output_should_stop)))
      return released
    monkeypatch.setattr(StoppedLeadRelease, 'sample', observed)
    results = []
    for motion in samples:
      _, _, held, plan = case.cycle(stopped=True, position=motion['model']['position']['x'],
        velocity=motion['model']['velocity']['x'], should_stop=False,
        desired_accel=motion['model']['action']['desiredAcceleration'], lead=True)
      results.append((held, plan.shouldStop, plan.aTarget))
    assert any(released for released, _, _, _ in observations), (observations, results)
    assert all(before == (target, stop) for _, before, target, stop in observations)
    assert any(not held and not stop and target > .1 for held, stop, target in results)
    assert not case.owner.mode.projector.stop_detector.committed
  finally:
    case.doCleanups()
