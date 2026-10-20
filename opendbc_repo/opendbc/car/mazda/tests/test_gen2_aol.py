"""CX-30 (GEN2) AOL: the host admits exactly the configuration the GEN2 safety profile was written and tested for."""
import pytest

from opendbc.can import CANPacker
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.stock_aol import GEN2_CARS, qualified, temporary_restriction
from opendbc.car.mazda.values import CAR, MazdaFlags
from openpilot.starpilot.aol.intent import AolSettings, ButtonType
from openpilot.starpilot.aol.vehicle import native_matches_cp, ordinary_axis_request_allowed, policy_for
from openpilot.starpilot.car.mazda import aol as mazda_aol
from openpilot.starpilot.car.mazda.stock_intent import MazdaStockCardIntent

MAZDA_MODEL = int(structs.CarParams.SafetyModel.mazda)


def cx30(experience=0):
  cp = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False)
  cp.alternativeExperience = experience
  return cp


def test_the_real_cx30_params_are_the_admitted_configuration():
  cp = cx30()
  assert GEN2_CARS == {CAR.MAZDA_CX_30}
  assert cp.safetyConfigs[0].safetyParam == int(MazdaFlags.GEN2) and int(cp.flags) == int(MazdaFlags.GEN2)
  assert cp.openpilotLongitudinalControl and cp.pcmCruise and not cp.dashcamOnly
  assert qualified(cp) and not qualified(cp, marked_only=True)
  assert qualified(cx30(32), marked_only=True) and qualified(cx30(32))


@pytest.mark.parametrize('mutation', (
  'brand', 'passive', 'not_car', 'dashcam', 'manual_gearbox', 'no_pcm_cruise', 'stock_long',
  'interceptor_word', 'bare_word', 'gen1_word', 'extra_config', 'other_model', 'experience_64', 'experience_96',
  'manual_flag', 'extra_flag'))
def test_every_deviation_from_the_tested_configuration_is_refused(mutation):
  cp = cx30(32)
  if mutation == 'brand':
    cp.brand = 'honda'
  elif mutation == 'passive':
    cp.passive = True
  elif mutation == 'not_car':
    cp.notCar = True
  elif mutation == 'dashcam':
    cp.dashcamOnly = True
  elif mutation == 'manual_gearbox':
    cp.transmissionType = structs.CarParams.TransmissionType.manual
  elif mutation == 'no_pcm_cruise':
    cp.pcmCruise = False
  elif mutation == 'stock_long':
    cp.openpilotLongitudinalControl = False
  elif mutation == 'interceptor_word':
    cp.safetyConfigs[0].safetyParam |= int(MazdaFlags.TORQUE_INTERCEPTOR)
  elif mutation == 'bare_word':
    cp.safetyConfigs[0].safetyParam = 0
  elif mutation == 'gen1_word':
    cp.safetyConfigs[0].safetyParam = int(MazdaFlags.GEN1)
  elif mutation == 'extra_config':
    cp.safetyConfigs = [cp.safetyConfigs[0], cp.safetyConfigs[0]]
  elif mutation == 'other_model':
    cp.safetyConfigs[0].safetyModel = structs.CarParams.SafetyModel.noOutput
  elif mutation == 'experience_64':
    cp.alternativeExperience = 64
  elif mutation == 'experience_96':
    cp.alternativeExperience = 96
  elif mutation == 'manual_flag':
    cp.flags |= int(MazdaFlags.MANUAL_TRANSMISSION)
  elif mutation == 'extra_flag':
    cp.flags |= int(MazdaFlags.NO_FSC)
  assert not qualified(cp) and not qualified(cp, marked_only=True)
  assert not policy_for(cp).settings_supported and not policy_for(cp).runtime_supported


@pytest.mark.parametrize('source', (structs.CarParams.FingerprintSource.can, structs.CarParams.FingerprintSource.fw,
                                    structs.CarParams.FingerprintSource.fixed))
def test_any_fingerprint_source_is_admitted_because_the_cx30_has_no_can_fingerprint(source):
  # The CX-30 is normally a selected (fixed) car; the recorded CarParams from the real drive logs say so.
  cp = cx30(32)
  cp.fingerprintSource = source
  assert qualified(cp) and qualified(cp, marked_only=True) and policy_for(cp).settings_supported


def test_gen1_cars_still_refuse_a_fixed_fingerprint():
  cp = CarInterface.get_params(CAR.MAZDA_CX5_2022, gen_empty_fingerprint(), [], False, False, False)
  cp.fingerprintSource = structs.CarParams.FingerprintSource.fixed
  cp.safetyConfigs[0].safetyParam = 0
  cp.flags = int(MazdaFlags.GEN1)
  assert not qualified(cp)


@pytest.mark.parametrize('model', (CAR.MAZDA_3_2019, CAR.MAZDA_CX_50, CAR.MAZDA_3_2023, CAR.MAZDA_CX_30_2023))
def test_other_gen2_and_gen3_models_stay_unadmitted(model):
  cp = CarInterface.get_params(model, gen_empty_fingerprint(), [], False, False, False)
  for experience in (0, 32):
    cp.alternativeExperience = experience
    assert not qualified(cp) and not policy_for(cp).settings_supported


def test_policy_exposes_lateral_only_with_the_ordinary_ack_when_marked():
  unmarked, marked = policy_for(cx30(0)), policy_for(cx30(32))
  for policy in (unmarked, marked):
    assert policy.intent_supported and policy.settings_supported and policy.runtime_supported
    assert policy.normal_runtime_supported and policy.explicit_latch
    assert policy.alternative_experience_addition == 32 and policy.safety_param_addition == 0
    assert not policy.full_axis_runtime_required
  assert not unmarked.ordinary_axis_ack_required and marked.ordinary_axis_ack_required


def test_native_profile_is_exactly_the_cp_safety_word():
  cp = cx30(32)
  assert mazda_aol.native_profile_supported(MAZDA_MODEL, 0) and mazda_aol.native_profile_supported(MAZDA_MODEL, 2)
  for word in (1, 3, 4, 6, 10, 34, 66, 130, 65535):
    assert not mazda_aol.native_profile_supported(MAZDA_MODEL, word)
  assert not mazda_aol.native_profile_supported(MAZDA_MODEL + 1, 2)
  assert native_matches_cp(cp, MAZDA_MODEL, 2) and mazda_aol.native_accepts_cp(cp, MAZDA_MODEL, 2)
  # word 0 is a supported profile, but it is not this vehicle's word
  assert not native_matches_cp(cp, MAZDA_MODEL, 0) and not mazda_aol.native_accepts_cp(cp, MAZDA_MODEL, 0)
  assert not native_matches_cp(cx30(0), MAZDA_MODEL, 2)  # unmarked CarParams never accept a native profile


def test_native_latch_rejection_and_ordinary_axis_permission_follow_the_marked_cp():
  cp, unmarked = cx30(32), cx30(0)
  denied = structs.CarState(canValid=True)
  native = type('Native', (), {'requestedLateral': True, 'lateralAllowed': False})()
  assert mazda_aol.native_latch_rejected(cp, native) and not mazda_aol.native_latch_rejected(unmarked, native)
  assert ordinary_axis_request_allowed(cp, denied) and not ordinary_axis_request_allowed(unmarked, denied)
  assert not ordinary_axis_request_allowed(cp, structs.CarState(canValid=True, canTimeout=True))
  assert not ordinary_axis_request_allowed(cp, structs.CarState(canValid=True, steerFaultPermanent=True))


def test_temporary_restrictions_cover_gear_standstill_and_faults():
  cp = cx30(32)
  drive = structs.CarState.GearShifter.drive
  assert not temporary_restriction(cp, structs.CarState(canValid=True, vEgo=20., gearShifter=drive))
  for gear in (structs.CarState.GearShifter.reverse, structs.CarState.GearShifter.park, structs.CarState.GearShifter.neutral):
    assert temporary_restriction(cp, structs.CarState(canValid=True, vEgo=20., gearShifter=gear))
  assert temporary_restriction(cp, structs.CarState(canValid=True, vEgo=0., standstill=True, gearShifter=drive))
  assert temporary_restriction(cp, structs.CarState(canValid=True, vEgo=20., gearShifter=drive, steerFaultTemporary=True))
  assert not temporary_restriction(cp, structs.CarState(canValid=True, canTimeout=True, vEgo=20., gearShifter=drive))
  assert not temporary_restriction(cx30(0), structs.CarState(canValid=True, vEgo=20., gearShifter=structs.CarState.GearShifter.park))


def test_actual_parser_reports_cancel_and_cruise_only_for_the_marked_profile():
  for experience in (0, 32):
    ci = CarInterface(cx30(experience))
    packer = CANPacker('mazda_2019')
    presses = []
    for tick, pressed in enumerate((0, 1, 1, 0), 1):
      frame = packer.make_can_msg('CRZ_BTNS', 0, {'CAN': pressed})
      state = ci.update([(tick * 10_000_000, [frame])])
      presses.extend(bool(e.pressed) for e in state.buttonEvents if e.type == structs.CarState.ButtonEvent.Type.cancel)
    assert presses == ([True, False] if experience == 32 else [])


@pytest.mark.parametrize('button', ('RES', 'SET_M', 'SET_P'))
def test_other_cruise_buttons_are_not_cancel(button):
  ci = CarInterface(cx30(32))
  packer = CANPacker('mazda_2019')
  for tick, pressed in enumerate((0, 1, 0), 1):
    state = ci.update([(tick * 10_000_000, [packer.make_can_msg('CRZ_BTNS', 0, {button: pressed})])])
    assert not [e for e in state.buttonEvents if e.type == structs.CarState.ButtonEvent.Type.cancel]


def test_actual_parser_cruise_availability_follows_crz_state():
  ci = CarInterface(cx30(32))
  packer = CANPacker('mazda_2019')
  seen = []
  for tick, crz_state in enumerate((0, 1, 2, 4, 0), 1):
    state = ci.update([(tick * 10_000_000, [packer.make_can_msg('CRUZE_STATE', 0, {'CRZ_STATE': crz_state})])])
    seen.append((state.cruiseState.available, state.cruiseState.enabled))
  assert seen == [(False, False), (True, False), (True, True), (True, True), (False, False)]


def test_stock_intent_latches_on_main_and_pauses_on_cancel_like_the_other_profiles():
  cp = cx30(32)
  owner = MazdaStockCardIntent(cp, AolSettings(True, 0., 0, 0, (0, 0, 0), (3, 0, 0)))
  state = structs.CarState(canValid=True, vEgo=20., gearShifter=structs.CarState.GearShifter.drive)
  owner.update(state, now_ns=1_000_000_000, fault_active=False)
  state.cruiseState.available = True
  owner.update(state, now_ns=1_010_000_000, fault_active=False)
  owner.update(state, now_ns=1_020_000_000, fault_active=False)
  assert owner.allowed_latch
  state.buttonEvents = [structs.CarState.ButtonEvent(type=ButtonType.cancel, pressed=True)]
  owner.update(state, now_ns=1_030_000_000, fault_active=False)
  state.buttonEvents = [structs.CarState.ButtonEvent(type=ButtonType.cancel, pressed=False)]
  owner.update(state, now_ns=1_040_000_000, fault_active=False)
  assert owner.pause_lateral and owner.allowed_latch
  state.cruiseState.available = False
  owner.update(state, now_ns=1_050_000_000, fault_active=False)
  assert not owner.allowed_latch


def test_cancel_wheel_rows_are_reported_for_the_marked_cx30_only():
  from openpilot.starpilot.controllers.wheel_actions import KEYS, button_reported
  for key in KEYS[5:8]:
    assert button_reported(cx30(32), key)
    assert not button_reported(cx30(0), key)
  assert not button_reported(cx30(32), 'LKASButtonControl') and not button_reported(cx30(32), 'MainCruiseButtonControl')


def test_dbc_layout_matches_what_the_firmware_decodes():
  packer = CANPacker('mazda_2019')
  # mazda.h reads CRZ_STATE as (data[0] & 0x70) >> 4 and the gear as (data[4] >> 1) & 0xF
  for crz_state in range(8):
    address, data, _ = packer.make_can_msg('CRUZE_STATE', 0, {'CRZ_STATE': crz_state})
    assert address == 0x44A and (data[0] & 0x70) >> 4 == crz_state
  for gear in range(16):
    address, data, _ = packer.make_can_msg('GEAR', 2, {'GEAR': gear})
    assert address == 0x228 and (data[4] >> 1) & 0xF == gear
