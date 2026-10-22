"""Dom's bus-0 radar CAN-filter path for the angle-control RAV4 (2023–25)."""
from opendbc.car import structs
from opendbc.car.toyota.values import CAR, ToyotaFlags

LONG_WORD = 0x1449
STOCK_WORD = 0x1649


def hardware_supported(candidate, fingerprint):
  # The filter's advertisement is a hardware discriminator, not an actuation
  # permission or a copy of ACC_CONTROL to decode under a different checksum.
  return (candidate == CAR.TOYOTA_RAV4_TSS2_2023 and fingerprint.get(0, {}).get(0x2AA) == 8 and
          not any(0x2AA in frames for bus, frames in fingerprint.items() if bus != 0) and
          not any(addr in frames for frames in fingerprint.values() for addr in (0x201, 0x2FF)))


def configured(cp):
  return bool(cp is not None and cp.brand == 'toyota' and cp.carFingerprint == CAR.TOYOTA_RAV4_TSS2_2023 and
              cp.flags & ToyotaFlags.RADAR_CAN_FILTER and not cp.flags & ToyotaFlags.DISABLE_RADAR and
              cp.steerControlType == structs.CarParams.SteerControlType.angle and len(cp.safetyConfigs) == 1 and
              cp.safetyConfigs[0].safetyModel == structs.CarParams.SafetyModel.toyota and
              cp.safetyConfigs[0].safetyParam == (LONG_WORD if cp.openpilotLongitudinalControl else STOCK_WORD))


def prepare_stock(cp):
  if not configured(cp):
    return False
  cp.openpilotLongitudinalControl = False
  cp.safetyConfigs[0].safetyParam = STOCK_WORD
  cp.autoResumeSng = False
  return True
