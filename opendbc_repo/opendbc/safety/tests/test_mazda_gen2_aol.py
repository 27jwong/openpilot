"""Mazda GEN2 (CX-30) AOL: actual RX -> ordinary grant -> brake -> retained AOL -> TX through the torque interceptor.

GEN2 steers by EPS_LKAS (0x249) on bus 1 and exposes neither the camera's LKAS state nor an EPS lockout flag, so cruise
state and gear are its only AOL sources. Everything else (brake, gas, speed, interceptor feedback) is the ordinary RX set.
"""
import unittest

from opendbc.car import structs
from opendbc.can.dbc import DBC
from opendbc.safety.tests.common import CANPackerSafety, make_msg
from opendbc.safety.tests.libsafety import libsafety_py

GEN2 = 2
DRIVE, REVERSE, PARK, NEUTRAL = 4, 12, 13, 11
OFF, READY, ENGAGED, GAS_OVERRIDE = 0, 1, 2, 4

# (name, bus) of every message the GEN2 profile consumes: five RX-checked, plus GEAR through the optional hook
SOURCES = (('CRUZE_STATE', 0), ('GEAR', 2), ('ENGINE_DATA', 2), ('SPEED', 2), ('BRAKE_PEDAL_SLOW', 0), ('TI_FEEDBACK', 1))


class TestMazdaGen2Aol(unittest.TestCase):
  def tearDown(self):
    self.safety.init_tests()
    self.safety.set_alternative_experience(0)
    self.safety.set_safety_hooks(structs.CarParams.SafetyModel.noOutput, 0)

  def configure(self, experience=32, word=GEN2):
    self.safety = libsafety_py.libsafety
    self.safety.init_tests()
    self.safety.set_alternative_experience(experience)
    self.safety.set_safety_hooks(structs.CarParams.SafetyModel.mazda, word)
    self.time = 1_000_000
    self.safety.set_timer(self.time)
    self.safety.set_aol_test_heartbeat(True)
    self.packer = CANPackerSafety('mazda_2019')
    self.dbc = DBC('mazda_2019')

  def packet(self, name, fields, bus=0):
    self.assertFalse(fields.keys() - self.dbc.name_to_msg[name].sigs.keys())
    return self.packer.make_can_msg_safety(name, bus, fields)

  def frame(self, *, state=READY, gear=DRIVE, brake=False, gas=False, speed=80, torque=0, omit=None, wrong_bus=None,
            request=1, tick=True):
    self.time = (self.time + 10_000) & 0xFFFFFFFF
    self.safety.set_timer(self.time)
    fields = {
      'CRUZE_STATE': {'CRZ_STATE': state},
      'GEAR': {'GEAR': gear},
      'ENGINE_DATA': {'PEDAL_GAS': int(gas)},
      'SPEED': {'SPEED': speed},
      'BRAKE_PEDAL_SLOW': {'BRAKE_PEDAL_PRESSED': int(brake)},
      'TI_FEEDBACK': {'STEER_TORQUE_SENSOR': torque},
    }
    for name, bus in SOURCES:
      if name != omit:
        self.safety.safety_rx_hook(self.packet(name, fields[name], (bus + 1) % 3 if name == wrong_bus else bus))
    if tick:
      self.safety.safety_tick()
    if request is not None:
      self.safety.aol_set_host_request(request)

  def warm(self, frames=20, **kwargs):
    for _ in range(frames):
      self.frame(**kwargs)

  def arm_with_main_switch(self):
    self.warm()
    self.frame(state=OFF)
    self.frame(state=READY)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def steer(self, torque, bus=1):
    return self.safety.safety_tx_hook(self.packet('EPS_LKAS', {'LKAS_REQUEST': torque, 'STEER_FEEL': 10000}, bus))

  def test_cold_main_on_has_no_permission_and_main_switch_arms(self):
    self.configure()
    self.warm()
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.assertFalse(self.steer(10))
    self.frame(state=OFF)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(state=READY)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)
    self.assertTrue(self.steer(10))

  def test_permission_only_for_the_requested_axis_and_never_without_a_request(self):
    self.configure()
    self.arm_with_main_switch()
    self.safety.aol_set_host_request(0)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.assertFalse(self.steer(10))
    self.safety.aol_set_host_request(2)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)  # longitudinal never rides a token
    self.safety.aol_set_host_request(1)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_torque_stays_inside_the_interceptor_limits(self):
    self.configure()
    self.arm_with_main_switch()
    self.assertTrue(self.steer(45))
    self.assertFalse(self.steer(45 + 46))  # more than 45 counts of rate up
    self.assertFalse(self.steer(8001))
    self.assertFalse(self.steer(10, bus=0))  # not the interceptor bus
    self.assertFalse(self.steer(10, bus=2))

  def test_stock_cruise_engagement_survives_brake_without_a_button(self):
    self.configure()
    self.warm()
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(state=ENGAGED)
    self.assertTrue(self.safety.get_controls_allowed())
    self.safety.aol_set_host_request(3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 3)
    self.frame(state=ENGAGED, brake=True, request=3)
    self.assertFalse(self.safety.get_controls_allowed())
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)  # lateral keeps its token, longitudinal does not
    self.assertTrue(self.steer(10))
    self.safety.aol_set_host_request(0)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_engaged_cruise_keeps_both_axes_at_a_standstill_and_in_any_gear(self):
    # openpilot longitudinal blends under stock cruise and the CX-30 steers at a standstill; AOL must not narrow either
    self.configure()
    self.warm()
    self.frame(state=ENGAGED, request=3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 3)
    for kwargs in ({'speed': 0}, {'gear': NEUTRAL}, {'gear': PARK, 'speed': 0}):
      with self.subTest(**kwargs):
        self.frame(state=ENGAGED, request=3, **kwargs)
        self.assertTrue(self.safety.get_controls_allowed())
        self.assertEqual(self.safety.aol_get_permission_mask(), 3)
        self.assertTrue(self.steer(10))
    # braking to a stop ends the ordinary engagement; the latch that outlives cruise keeps lateral at the standstill
    self.frame(state=ENGAGED, brake=True, speed=0, request=3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)
    self.frame(state=READY, request=3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_real_engagement_order_grant_before_heartbeat_then_post_grant_request(self):
    # On the car the stock cruise engages first; openpilot's heartbeat and its first lateral request follow it.
    self.configure()
    self.safety.set_aol_test_heartbeat(False)
    self.warm(request=0)
    self.frame(state=ENGAGED, request=0)
    self.assertTrue(self.safety.get_controls_allowed())
    self.assertEqual(self.safety.aol_get_request_mask(), 0)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.safety.set_aol_test_heartbeat(True)
    self.frame(state=ENGAGED, request=0)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.time += 20_000
    self.safety.set_timer(self.time)
    self.frame(state=ENGAGED, request=3)
    self.assertEqual(self.safety.aol_get_request_mask(), 3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 3)
    # the latch survives the brake only because the post-grant request arrived inside the pending window
    self.frame(state=ENGAGED, brake=True, request=3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_a_grant_never_followed_by_a_request_expires_instead_of_latching(self):
    self.configure()
    self.safety.set_aol_test_heartbeat(False)
    self.warm(request=0)
    self.frame(state=ENGAGED, request=0)
    for _ in range(40):
      self.frame(state=ENGAGED, request=0)
    self.safety.set_aol_test_heartbeat(True)
    self.frame(state=ENGAGED, brake=True, request=1)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_engagement_still_needs_a_healthy_handshake(self):
    self.configure()
    self.warm()
    self.frame(state=ENGAGED, request=3)
    self.assertEqual(self.safety.aol_get_permission_mask(), 3)
    self.safety.set_aol_test_heartbeat(False)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.safety.set_aol_test_heartbeat(True)
    self.frame(state=ENGAGED, request=3, omit='GEAR')
    for _ in range(40):
      self.frame(state=ENGAGED, request=3, omit='GEAR')
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)  # a lost gear source withdraws every axis

  def test_cruise_already_engaged_at_startup_never_grants_a_token(self):
    self.configure()
    self.warm(state=ENGAGED)
    self.assertTrue(self.safety.get_controls_allowed())
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)  # ordinary engagement is permission enough
    self.frame(state=ENGAGED, brake=True)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(state=ENGAGED)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_engaging_with_the_pedal_down_is_ordinary_engagement_but_cannot_grant(self):
    self.configure()
    self.warm()
    self.warm(frames=3, gas=True)
    self.frame(state=GAS_OVERRIDE, gas=True)
    self.assertTrue(self.safety.get_controls_allowed())
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)  # controls_allowed, as in the ordinary profile
    self.frame(state=READY, gas=True)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)  # no token was ever minted

  def test_main_off_revokes_and_requires_a_new_edge(self):
    self.configure()
    self.arm_with_main_switch()
    self.frame(state=OFF)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(state=READY)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)  # the edge is a fresh physical grant
    self.warm()
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_gear_other_than_drive_pauses_without_destroying_the_token(self):
    self.configure()
    self.arm_with_main_switch()
    for gear in (REVERSE, PARK, NEUTRAL, 14, 0, 7, 15):
      with self.subTest(gear=gear):
        self.frame(gear=gear)
        self.assertEqual(self.safety.aol_get_permission_mask(), 0)
        self.assertFalse(self.steer(10))
        self.frame()
        self.assertEqual(self.safety.aol_get_permission_mask(), 1)
    for gear in range(1, 7):
      self.frame(gear=gear)
      self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_standstill_keeps_the_latched_lateral(self):
    # The CX-30 steers at a standstill. Withdrawing permission at a stop made the host drop the latch (route
    # 00000002--e9815ca2fd, 197.84 s), so a stop must not change what the latch is allowed to do.
    self.configure()
    self.arm_with_main_switch()
    for speed in (0, 80, 0):
      self.frame(speed=speed)
      self.assertEqual(self.safety.aol_get_permission_mask(), 1)
    self.frame(speed=0, gear=NEUTRAL)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(speed=0)
    self.assertEqual(self.safety.aol_get_permission_mask(), 1)

  def test_each_required_source_missing_wrong_bus_or_length_expires(self):
    for name, _ in SOURCES:
      for fault in ('missing', 'bus', 'length'):
        with self.subTest(source=name, fault=fault):
          self.configure()
          self.arm_with_main_switch()
          for _ in range(120):
            self.frame(omit=name if fault != 'bus' else None, wrong_bus=name if fault == 'bus' else None)
            if fault == 'length':
              self.safety.safety_rx_hook(make_msg(dict(SOURCES)[name], self.dbc.name_to_msg[name].address, dat=bytes(7)))
          self.assertEqual(self.safety.aol_get_permission_mask(), 0)
          self.frame()
          self.assertEqual(self.safety.aol_get_permission_mask(), 0)  # recovery of the source is not a new grant
          self.arm_with_main_switch()

  def test_gear_on_the_main_bus_is_not_the_gear_the_host_reads(self):
    self.configure()
    self.warm(wrong_bus='GEAR')
    self.frame(state=OFF, wrong_bus='GEAR')
    self.frame(state=READY, wrong_bus='GEAR')
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_lost_session_or_request_never_synthesizes_a_rearm(self):
    self.configure()
    self.arm_with_main_switch()
    self.safety.set_aol_test_heartbeat(False)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.safety.set_aol_test_heartbeat(True)
    self.frame()
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.arm_with_main_switch()
    self.time += 300_001
    self.safety.set_timer(self.time)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.frame(request=None)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_ordinary_profile_is_unchanged_without_the_aol_experience(self):
    self.configure(experience=0)
    self.warm()
    self.frame(state=OFF)
    self.frame(state=READY)
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)
    self.assertFalse(self.steer(10))  # not engaged
    self.frame(state=ENGAGED)
    self.assertTrue(self.safety.get_controls_allowed())
    self.assertTrue(self.steer(10))
    self.frame(state=ENGAGED, brake=True)
    self.assertFalse(self.safety.get_controls_allowed())
    self.assertFalse(self.steer(10))

  def test_only_the_gen2_word_enables_the_profile(self):
    for word in (1, 3, 4, 6, 10, 18, 34, 66, 130, 2 | 8, 65535):
      with self.subTest(word=word):
        self.configure(word=word)
        self.warm()
        self.frame(state=OFF)
        self.frame(state=READY)
        self.assertEqual(self.safety.aol_get_permission_mask(), 0)
        self.assertFalse(self.steer(10))  # AOL experience on an unqualified word blanks the TX list

  def test_unrelated_traffic_on_the_aux_bus_cannot_grant(self):
    self.configure()
    self.warm()
    self.frame(state=OFF)
    self.safety.safety_rx_hook(self.packet('CRUZE_STATE', {'CRZ_STATE': READY}, bus=1))
    self.safety.safety_rx_hook(self.packet('CRUZE_STATE', {'CRZ_STATE': READY}, bus=2))
    self.assertEqual(self.safety.aol_get_permission_mask(), 0)

  def test_does_not_forward_or_hold_the_camera_lkas_path(self):
    self.configure()
    self.arm_with_main_switch()
    self.assertTrue(self.steer(10))
    for bus, address in ((2, 0x243), (2, 0x440), (0, 0x243)):
      self.assertNotEqual(self.safety.safety_fwd_hook(bus, address), -1)


if __name__ == "__main__":
  unittest.main()
