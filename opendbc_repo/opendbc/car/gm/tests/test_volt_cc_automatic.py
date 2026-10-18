"""Source-qualified Volt CC discovery reaches the existing parser/controller/native owner."""
import unittest
from types import SimpleNamespace
from unittest.mock import patch

from opendbc.can import CANPacker
from opendbc.car import Bus, structs
from opendbc.car.car_helpers import can_fingerprint
from opendbc.car.gm.fingerprints import FINGERPRINTS
from opendbc.car.gm.interface import CarInterface
from opendbc.car.gm.tests import test_volt_cc_gas as cc_fixture
from opendbc.car.gm.values import CAR, DBC, GMFlags
from opendbc.safety.tests.libsafety import libsafety_py


class TestVoltCcAutomatic(unittest.TestCase):
  def discovered(self, release=False):
    variants = FINGERPRINTS[CAR.CHEVROLET_VOLT_CC]
    self.assertEqual(len(variants), 1)
    messages = [SimpleNamespace(address=address, dat=bytes(length), src=0)
                for address, length in variants[0].items()]
    selected, observed = can_fingerprint(lambda **_kwargs: [messages])
    self.assertEqual(selected, CAR.CHEVROLET_VOLT_CC)
    cp = CarInterface.get_params(selected, observed, [], False, release, False)
    return cp, CarInterface(cp), CANPacker(DBC[cp.carFingerprint][Bus.pt])

  def test_actual_discovery_finalizes_existing_owner_and_release_denial(self):
    for release in (False, True):
      cp, ci, _ = self.discovered(release)
      self.assertEqual(cp.carFingerprint, CAR.CHEVROLET_VOLT_CC)
      self.assertEqual(cp.openpilotLongitudinalControl, not release)
      self.assertEqual(cp.dashcamOnly, release)
      self.assertEqual(bool(cp.flags & GMFlags.CC_LONG), not release)
      self.assertEqual(ci.CC.volt_cc_profile, not release)
      if not release:
        self.assertEqual(cp.safetyConfigs[0].safetyParam, 20)
      # Physical structure comes from this car, not another GM tune.
      reference = cc_fixture.fixture()[0]
      self.assertEqual(cp.mass, reference.mass)
      self.assertEqual(cp.wheelbase, reference.wheelbase)
      self.assertEqual(cp.lateralTuning.to_dict(), reference.lateralTuning.to_dict())

  def test_discovered_owner_emits_real_set_and_native_enforces_credit(self):
    # Reuse the existing real receive/ADC/gear/button sequence; replace only discovery.
    probe = cc_fixture.TestVoltCcGas()
    safety = libsafety_py.libsafety
    release = safety.set_safety_hooks(structs.CarParams.SafetyModel.allOutput, 0) != 0
    # Reinitialize the actual GM profile after detecting build flavor.
    with patch.object(cc_fixture, 'fixture', return_value=self.discovered()):
      _, ci, _, now = probe.prepare()
    ci.CC.frame = 156
    _, packets = ci.apply(probe.command().as_reader(), now + 1)
    cancel_or_set = [packet for packet in packets if packet[0] == 0x1E1]
    self.assertEqual(len(cancel_or_set), 1)
    packet = cancel_or_set[0]
    self.assertEqual(packet[2], 0)
    self.assertEqual((packet[1][5] >> 4) & 7, 3)
    self.assertEqual(cc_fixture.native('tx', packet, now // 1000), not release)
    self.assertFalse(cc_fixture.native('tx', packet, now // 1000))

  def test_automatic_card_factory_and_controls_preserve_disabled_startup(self):
    import json
    import os

    from opendbc.car import car_helpers
    from opendbc.car.can_definitions import CanData
    from openpilot.common.params import Params
    from openpilot.common.prefix import OpenpilotPrefix
    from openpilot.selfdrive.car.card import Car
    from openpilot.selfdrive.controls.controlsd import Controls
    from openpilot.starpilot.lateral.tests.test_lane_runtime import feed
    from openpilot.starpilot.vehicle_selection import encode

    physical = FINGERPRINTS[CAR.CHEVROLET_VOLT_CC][0]
    packets = [CanData(address, bytes(length), 0) for address, length in physical.items()]

    def receive(wait_for_one=False):
      return [packets] if wait_for_one else []

    def discover(*args, **kwargs):
      return car_helpers.get_car(receive, *args[1:], **kwargs)
    with OpenpilotPrefix(), patch.dict(os.environ, {'SKIP_FW_QUERY': '1', 'FINGERPRINT': '', 'SIMULATION': '1', 'REPLAY': '1'}):
      settings = Params()
      settings.put_bool('OpenpilotEnabledToggle', True, block=True)
      settings.put_bool('IsReleaseBranch', False, block=True)
      settings.put('VehicleSelection', json.loads(encode(None)), block=True)
      with patch('openpilot.selfdrive.car.card.messaging.recv_one_retry', return_value=SimpleNamespace(can=[1])), \
           patch('openpilot.selfdrive.car.card.get_car', side_effect=discover):
        card = Car()
      self.assertEqual(card.CP.carFingerprint, CAR.CHEVROLET_VOLT_CC)
      self.assertEqual(card.CP.fingerprintSource, structs.CarParams.FingerprintSource.can)
      self.assertTrue(card.CI.CC.volt_cc_profile)
      self.assertFalse(card.CP.dashcamOnly)
      controls = Controls()
      self.assertEqual(controls.CP.carFingerprint, card.CP.carFingerprint)
      self.assertEqual(controls.CP.safetyConfigs[0].safetyParam, 20)
      feed(controls, 1_000_000_000, 0, active=False, enabled=False, can_valid=False, can_timeout=True)
      command, _ = controls.state_control()
      self.assertFalse(command.enabled)
      self.assertFalse(command.latActive)
      self.assertFalse(command.longActive)


if __name__ == '__main__':
  unittest.main()
