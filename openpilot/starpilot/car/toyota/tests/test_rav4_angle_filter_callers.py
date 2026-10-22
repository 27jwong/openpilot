"""Matched FW and real Card/SelfdriveD/Controls/CI/native path for RAV4 radar filter.

CAN, firmware acquisition, perception and USB are synthetic. A shared 100 Hz
clock tests source/native ordering; this is not installed-hardware acceptance.
"""
import json
import time
from contextlib import ExitStack
from unittest.mock import patch

import pytest

from opendbc.can import CANPacker
from opendbc.car import Bus, car_helpers, gen_empty_fingerprint, structs
from opendbc.car.fw_versions import match_fw_to_car
from opendbc.car.toyota.values import CAR, DBC, ToyotaFlags
from opendbc.car.toyota.tests.test_auto_hold import decode
from opendbc.safety.tests.libsafety import libsafety_py
from openpilot.cereal import messaging
from openpilot.common.params import Params
from openpilot.common.prefix import OpenpilotPrefix
from openpilot.selfdrive.car import card as card_module
from openpilot.selfdrive.controls.controlsd import Controls
from openpilot.selfdrive.selfdrived import selfdrived as sd_module
from openpilot.starpilot.car.toyota.tests import test_pedal_caller_native as helpers

source_matched_native = helpers.source_matched_native
daemon_gc_scope = helpers.daemon_gc_scope


@pytest.mark.parametrize('alpha,disable,radar_mode', [(False, False, 0), (True, False, 0), (True, False, 1), (True, False, 2), (True, True, 1)])
def test_rav4_filter_actual_callers(alpha, disable, radar_mode, tmp_path):
  run_case(CAR.TOYOTA_RAV4_TSS2_2023, alpha, disable, radar_mode, tmp_path)


def run_case(candidate, alpha, disable, radar_mode, tmp_path, *, aol=True):
  helpers.SYNTHETIC_LAST.clear()
  helpers.NATIVE_LAST.clear()
  safety = libsafety_py.libsafety
  trace = []
  with OpenpilotPrefix(), ExitStack() as stack:
    params = Params()
    for key, value in [('OpenpilotEnabledToggle', True), ('AlphaLongitudinalEnabled', alpha),
                       ('DisableOpenpilotLongitudinal', disable), ('AlwaysOnLateral', aol), ('SafeMode', False)]:
      params.put_bool(key, value, block=True)
    params.put('ToyotaRadarMode', radar_mode, block=True)
    fw = helpers.firmware(candidate)
    exact, found = match_fw_to_car(fw, '', log=False)
    assert exact and found == {candidate}
    fp = gen_empty_fingerprint()
    fp[0][0x2AA] = 8
    observed = (candidate, fp, '0' * 17, fw, structs.CarParams.FingerprintSource.fw, True)
    with (patch.object(car_helpers, 'fingerprint', return_value=observed),
          patch.object(card_module, 'can_comm_callbacks', return_value=(lambda *a: [], lambda x: None)),
          patch.object(card_module.messaging, 'recv_one_retry', return_value=messaging.new_message('can', 1))):
      host = card_module.Car()
    long = alpha and not disable
    assert not host.CP.flags & ToyotaFlags.DISABLE_RADAR
    angle = candidate == CAR.TOYOTA_RAV4_TSS2_2023
    enrolled = not angle and aol
    assert host.CP.safetyConfigs[0].safetyParam == ((0x1449 if long else 0x1649) if angle else (0x1049 if long else 0x1249))
    assert host.CP.openpilotLongitudinalControl == long
    # This port keeps the existing angle-control AOL enrollment boundary.
    assert host.CP.alternativeExperience == (0 if angle else 32)
    controls, sd = Controls(), sd_module.SelfdriveD(CP=host.CP)
    assert controls.CP.to_dict() == host.CP.to_dict()
    safety.init_tests()
    safety.set_alternative_experience(host.CP.alternativeExperience)
    assert safety.set_safety_hooks(host.CP.safetyConfigs[0].safetyModel.raw, host.CP.safetyConfigs[0].safetyParam) == 0
    safety.set_aol_test_heartbeat(True)
    packer = CANPacker(DBC[candidate][Bus.pt])
    published, previous = {}, {}

    def capture(name, message):
      if isinstance(message, bytes):
        message = messaging.log_from_bytes(message)
      published[name] = message.as_reader() if hasattr(message, 'as_reader') else message

    stack.enter_context(patch.object(host.pm, 'send', side_effect=capture))
    stack.enter_context(patch.object(controls.pm, 'send', side_effect=capture))
    clock = [time.monotonic_ns()]
    real_clock = time.clock_gettime_ns
    stack.enter_context(patch.object(time, 'monotonic_ns', side_effect=lambda: clock[0]))
    stack.enter_context(patch.object(time, 'monotonic', side_effect=lambda: clock[0] / 1e9))
    stack.enter_context(patch.object(time, 'clock_gettime_ns',
      side_effect=lambda c: clock[0] if c in (time.CLOCK_MONOTONIC, time.CLOCK_BOOTTIME) else real_clock(c)))
    cc = structs.CarControl().as_reader()
    active = steering = acceleration = False
    try:
      for tick in range(1, 651):
        clock[0] += 10_000_000
        now = clock[0]
        safety.set_timer((now // 1000) & 0xFFFFFFFF)
        brake = 260 <= tick < 280
        gas = 310 <= tick < 330
        reverse = 360 <= tick < 380
        fault = 420 <= tick < 440
        main = not 500 <= tick < 520
        cruise = tick >= 150 and not any(a <= tick < b for a, b in ((260, 290), (310, 340), (360, 390), (420, 450), (500, 530)))
        frames = helpers.physical(packer, cruise=cruise, main=main, brake=brake, gas=gas, stopped=brake, gear=16 if reverse else 0, eps=9 if fault else 1)
        # LTA requires initialized measured angle and its own EPS status.
        frames = [f for f in frames if f[0] not in (0x260, 0x262)]
        frames += [packer.make_can_msg('STEER_TORQUE_SENSOR', 0, {'STEER_ANGLE': .0573}),
                   packer.make_can_msg('EPS_STATUS', 0, {'LKA_STATE': 9 if fault else 1, 'LTA_STATE': 9 if fault else 1}),
                   packer.make_can_msg('PCS_HUD', 0, {}),
                   # The physical filter advertises itself; it does not grant controls.
                   (0x2AA, bytes(8), 0)]
        # Stock AEB/PCS traffic must not trip the ACC relay check.
        if 580 <= tick < 600:
          frames.append(packer.make_can_msg('ACC_CONTROL', 0, {}))
        for address, data, bus in frames:
          assert safety.safety_rx_hook(libsafety_py.make_CANPacket(address, bus, data)), (tick, hex(address))
        safety.safety_tick()
        native = helpers.native_events(host, safety, now, sd.aol_session_id)
        helpers.deliver(host.sm, [*native, *previous.values()])
        batch = messaging.log_from_bytes(card_module.can_list_to_can_capnp(frames)).as_builder()
        batch.logMonoTime = now
        with patch.object(card_module.messaging, 'drain_sock_raw', return_value=[batch.to_bytes()]), patch.object(host.sm, 'update'):
          cs, radar = host.state_update()
        host.state_publish(cs, radar)
        state = dict(published)
        peers = helpers.selfdrive_inputs(sd, state, native, cc, host.CP, now)
        events = {}
        with (patch.object(sd_module.messaging, 'recv_one', return_value=state['carState']),
              patch.object(sd_module.messaging, 'recv_one_or_none', return_value=None),
              patch.object(sd.sm, 'update', side_effect=lambda _, peers=peers: helpers.deliver(sd.sm, peers)),
              patch.object(sd.pm, 'send', side_effect=lambda n, m, events=events: events.update({n: m.as_reader()}))):
          sd.step()
        wanted = sd.aol_axis_decision
        safety.aol_set_host_request(int(wanted.desired_lateral) | int(wanted.desired_longitudinal) << 1)
        native = helpers.native_events(host, safety, now, sd.aol_session_id)
        current = {**previous, **events}
        helpers.deliver(controls.sm, helpers.control_inputs(controls, published, state, current, native, now, .00005))
        command, lac_log = controls.state_control()
        controls.publish(command, lac_log)
        cc_event = published['carControl']
        cc = cc_event.carControl
        helpers.deliver(host.sm, [cc_event, *native, *events.values()])
        host.controls_update(cs, cc)
        outgoing = published['sendcan']
        for frame in outgoing.sendcan:
          assert frame.address != 0x750, 'Filter path must not keep radar disabled'
          packet = libsafety_py.make_CANPacket(frame.address, frame.src, bytes(frame.dat))
          assert safety.safety_tx_hook(packet), (tick, hex(frame.address), bytes(frame.dat).hex())
          if frame.address == 0x191:
            value = decode(host.CP, (frame.address, bytes(frame.dat), frame.src), 'STEERING_LTA')
            steering |= bool(value['STEER_REQUEST'] and abs(value['STEER_ANGLE_CMD']) > 0)
          elif frame.address == 0x2E4 and not angle:
            value = decode(host.CP, (frame.address, bytes(frame.dat), frame.src), 'STEERING_LKA')
            steering |= bool(value['STEER_REQUEST'] and value['STEER_TORQUE_CMD'])
          elif frame.address == 0x343:
            value = decode(host.CP, (frame.address, bytes(frame.dat), frame.src), 'ACC_CONTROL')
            acceleration |= value['ACCEL_CMD'] > 0
        trace.append({'tick': tick, 'canValid': cs.canValid, 'lat': cc.latActive, 'long': cc.longActive,
                      'events': [str(e) for e in sd.events.names], 'state': str(sd.state_machine.state)})
        if any(a <= tick < b for a, b in ((220, 240), (550, 570), (620, 640))):
          assert cs.canValid and not cs.canTimeout and host.ci_initialized and sd.initialized, trace[-1]
          assert cc.latActive and cc.longActive == long, trace[-1]
          active = True
        if any(a <= tick < b for a, b in ((370, 379), (430, 439), (510, 519))):
          assert not cc.latActive and not cc.longActive, trace[-1]
        if any(a <= tick < b for a, b in ((270, 279), (320, 329))):
          assert cc.latActive == enrolled and not cc.longActive, trace[-1]
        if enrolled and 442 <= tick < 449:
          assert cc.latActive and not cc.longActive, trace[-1]
        previous = current
      assert active and steering
      assert acceleration == long
      assert not safety.get_relay_malfunction()
    finally:
      (tmp_path / 'trace.json').write_text(json.dumps(trace, indent=2) + '\n')
      safety.set_alternative_experience(0)
      safety.set_safety_hooks(structs.CarParams.SafetyModel.noOutput, 0)
      host.vehicle_startup.close()
