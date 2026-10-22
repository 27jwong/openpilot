"""Exact stock-cruise Subaru independent-lateral configuration contract."""
from opendbc.car import structs
from opendbc.car.subaru.values import CAR, SubaruFlags, SubaruSafetyFlags

AOL_EXPERIENCE = 32
TORQUE_CARS = frozenset((CAR.SUBARU_ASCENT, CAR.SUBARU_OUTBACK, CAR.SUBARU_LEGACY,
                        CAR.SUBARU_IMPREZA, CAR.SUBARU_IMPREZA_2020, CAR.SUBARU_FORESTER))
ANGLE_CARS = frozenset((CAR.SUBARU_CROSSTREK_2025, CAR.SUBARU_LEGACY_2025, CAR.SUBARU_ASCENT_2023))


def qualified(cp, *, marked_only=False):
  if (cp is None or cp.brand != 'subaru' or cp.carFingerprint not in TORQUE_CARS | ANGLE_CARS or
      cp.notCar or cp.passive or cp.dashcamOnly or not cp.pcmCruise or cp.openpilotLongitudinalControl or
      cp.transmissionType == structs.CarParams.TransmissionType.manual or len(cp.safetyConfigs) != 1 or
      cp.alternativeExperience not in ((AOL_EXPERIENCE,) if marked_only else (0, AOL_EXPERIENCE))):
    return False
  static = int(CAR[cp.carFingerprint].config.flags)
  allowed = int(SubaruFlags.HAS_BSM | SubaruFlags.SEND_INFOTAINMENT)
  expected = int(SubaruSafetyFlags.GEN2) if static & SubaruFlags.GLOBAL_GEN2 else 0
  angle = cp.carFingerprint in ANGLE_CARS
  if angle:
    expected |= int(SubaruSafetyFlags.LKAS_ANGLE)
    if cp.carFingerprint != CAR.SUBARU_CROSSTREK_2025:
      expected |= int(SubaruSafetyFlags.FIXED_ANGLE_LIMITS)
    if cp.carFingerprint == CAR.SUBARU_ASCENT_2023:
      expected |= int(SubaruSafetyFlags.ANGLE_MAIN_BUS)
      allowed |= int(SubaruFlags.ASCENT_LKAS_ALERT)
      if cp.flags & SubaruFlags.ASCENT_LKAS_ALERT:
        expected |= int(SubaruSafetyFlags.ASCENT_LKAS_ALERT)
  if (int(cp.flags) & ~allowed != static or
      cp.steerControlType != (structs.CarParams.SteerControlType.angle if angle else structs.CarParams.SteerControlType.torque)):
    return False
  config = cp.safetyConfigs[0]
  return config.safetyModel == structs.CarParams.SafetyModel.subaru and config.safetyParam == expected


def temporary_restriction(cp, state):
  return bool(state is not None and qualified(cp, marked_only=True) and state.canValid and not state.canTimeout and
              not state.steerFaultPermanent and not state.accFaulted and state.cruiseState.available and
              (state.gearShifter != structs.CarState.GearShifter.drive or state.steerFaultTemporary or state.standstill))
