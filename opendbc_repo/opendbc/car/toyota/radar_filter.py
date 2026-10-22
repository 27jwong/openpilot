"""Dom's bus-0 radar CAN-filter paths for qualified RAV4, Camry and C-HR configurations."""
from opendbc.car import structs
from opendbc.car.toyota.values import CAR, ToyotaFlags

LONG_WORD = 0x1449
STOCK_WORD = 0x1649
PROFILES = {
  CAR.TOYOTA_CAMRY: (structs.CarParams.SteerControlType.torque, 0x1049, 0x1249),
  CAR.TOYOTA_CHR: (structs.CarParams.SteerControlType.torque, 0x1049, 0x1249),
  CAR.TOYOTA_CHR_TSS2: (structs.CarParams.SteerControlType.torque, 0x1049, 0x1249),
  CAR.TOYOTA_RAV4_TSS2_2022: (structs.CarParams.SteerControlType.torque, 0x1049, 0x1249),
  CAR.TOYOTA_RAV4_TSS2_2023: (structs.CarParams.SteerControlType.angle, LONG_WORD, STOCK_WORD),
}


def hardware_supported(candidate, fingerprint):
  # The filter's advertisement is a hardware discriminator, not an actuation
  # permission or a copy of ACC_CONTROL to decode under a different checksum.
  return (candidate in PROFILES and fingerprint.get(0, {}).get(0x2AA) == 8 and
          not any(0x2AA in frames for bus, frames in fingerprint.items() if bus != 0) and
          not any(addr in frames for frames in fingerprint.values() for addr in (0x201, 0x2FF)))


def configured(cp):
  profile = PROFILES.get(cp.carFingerprint) if cp is not None else None
  return bool(profile is not None and cp.brand == 'toyota' and
              cp.flags & ToyotaFlags.RADAR_CAN_FILTER and not cp.flags & ToyotaFlags.DISABLE_RADAR and
              cp.steerControlType == profile[0] and len(cp.safetyConfigs) == 1 and
              cp.safetyConfigs[0].safetyModel == structs.CarParams.SafetyModel.toyota and
              cp.safetyConfigs[0].safetyParam == (profile[1] if cp.openpilotLongitudinalControl else profile[2]))


def prepare_stock(cp):
  if not configured(cp):
    return False
  cp.openpilotLongitudinalControl = False
  cp.safetyConfigs[0].safetyParam = PROFILES[cp.carFingerprint][2]
  cp.autoResumeSng = False
  return True
