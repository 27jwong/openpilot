from opendbc.car import structs
from opendbc.car.mazda.values import CAR, MazdaFlags

# GEN1 steers through the stock camera's LKAS message; GEN2 through the torque interceptor on bus 1, with the
# platform flag as the whole safety word. Each is admitted only for the exact configuration its firmware profile expects.
GEN1_CARS = frozenset((CAR.MAZDA_CX5_2022, CAR.MAZDA_CX9_2021))
GEN2_CARS = frozenset((CAR.MAZDA_CX_30,))
CARS = GEN1_CARS | GEN2_CARS


def qualified(cp, *, marked_only=False):
  if cp.carFingerprint in GEN2_CARS:
    # The CX-30 has no CAN fingerprint, so a fixed (selected) car is how it is normally identified.
    flags, safety_param, openpilot_long, fixed_allowed = MazdaFlags.GEN2, int(MazdaFlags.GEN2), True, True
  else:
    flags, safety_param, openpilot_long, fixed_allowed = MazdaFlags.GEN1, 0, False, False
  return (cp.brand == "mazda" and cp.carFingerprint in CARS and int(cp.flags) == int(flags) and
          not cp.passive and not cp.notCar and not cp.dashcamOnly and
          cp.transmissionType != structs.CarParams.TransmissionType.manual and
          (fixed_allowed or cp.fingerprintSource != structs.CarParams.FingerprintSource.fixed) and
          cp.pcmCruise and bool(cp.openpilotLongitudinalControl) == openpilot_long and len(cp.safetyConfigs) == 1 and
          cp.safetyConfigs[0].safetyModel == structs.CarParams.SafetyModel.mazda and
          cp.safetyConfigs[0].safetyParam == safety_param and
          cp.alternativeExperience in ((32,) if marked_only else (0, 32)))


def temporary_restriction(cp, state):
  return bool(state is not None and qualified(cp, marked_only=True) and state.canValid and not state.canTimeout and
              not state.steerFaultPermanent and (state.gearShifter != structs.CarState.GearShifter.drive or
                                                 state.steerFaultTemporary or state.standstill or (cp.minSteerSpeed > 0 and state.vEgo < cp.minSteerSpeed)))


def native_observation(native, *, latched, panda_ready, restricted, now_ns, pending_since_ns):
  lost = not panda_ready or native is None
  if not latched or lost or restricted:
    return 0, lost, False
  if native.requestedLateral and native.lateralAllowed:
    return 0, False, False
  pending_since_ns = pending_since_ns or now_ns
  return pending_since_ns, False, now_ns - pending_since_ns >= 200_000_000
