from types import SimpleNamespace
from unittest.mock import patch

import pytest

from opendbc.car import structs
from opendbc.car.hyundai.tests.test_ioniq5pe_parked_ready import ParkedFixture
from opendbc.safety.tests.libsafety import libsafety_py


@pytest.mark.parametrize('withdrawal', ['cancel', 'main', 'brake'])
def test_parked_ready_pending_neutral_native_handoff_requires_new_set_then_obeys_withdrawal(withdrawal):
  fixture = ParkedFixture()
  native = libsafety_py.libsafety
  native.set_alternative_experience(0)
  assert native.set_safety_hooks(structs.CarParams.SafetyModel.elm327, 1) == 0
  native.init_tests()

  def packet(frame):
    return libsafety_py.make_CANPacket(frame[0], frame[2], frame[1])
  with patch('time.clock_gettime_ns', side_effect=lambda clock: fixture.ns):
    try:
      fixture.prepare()
      fixture.warm()
      fixture.commit()
      assert native.set_safety_hooks(structs.CarParams.SafetyModel.hyundaiCanfd, 0x5495) == 0

      def tick():
        batch = fixture.recv()[0]
        native.set_timer(fixture.ns // 1000)
        for frame in batch:
          accepted = native.safety_rx_hook(packet(frame))
          if frame[0] in (0x35, 0x175, 0xA0, 0xEA, 0x1CF):
            assert accepted, hex(frame[0])
        fixture.ci.update([(batch.log_mono_time_ns, list(batch))])
        cfg = fixture.ci.CP.safetyConfigs[0]
        panda = SimpleNamespace(safetyModel=cfg.safetyModel, safetyParam=cfg.safetyParam,
                                alternativeExperience=0, faults=[], safetyRxChecksInvalid=False,
                                ignitionLine=True, ignitionCan=False)
        fixture.owner.after_state(ci=fixture.ci, state=fixture.ci.CS.out, now_ns=fixture.ns,
                                  control_enabled=False, control_current=True, pandas=[panda],
                                  panda_log_ns=fixture.ns, panda_recv_ns=fixture.ns, panda_current=True)
      for _ in range(15):
        tick()
      assert not native.get_controls_allowed()
      cc = structs.CarControl(enabled=True, latActive=True, longActive=True)
      cc.actuators.accel = 1.
      cc.actuators.steeringAngleDeg = 1.
      cc.actuators.longControlState = structs.CarControl.Actuators.LongControlState.pid
      _, neutral = fixture.ci.apply(cc.as_reader(), fixture.ns)
      assert {0x1A0, 0xCB, 0x100, 0x730}.issubset({frame[0] for frame in neutral})
      assert ((next(frame[1] for frame in neutral if frame[0] == 0x1A0)[8] >> 4) & 7) == 0
      assert ((next(frame[1] for frame in neutral if frame[0] == 0xCB)[3] >> 4) & 15) == 1
      for frame in neutral:
        assert native.safety_tx_hook(packet(frame)), hex(frame[0])
      fixture.owner.sent(neutral, valid=True)
      assert fixture.owner.handed_off and fixture.owner.thread.joined
      fixture.gear, fixture.moving = 5, True
      for _ in range(15):
        tick()
      assert not native.get_controls_allowed()
      denied = fixture.packer.make_can_msg('SCC_CONTROL', 1, {'ACCMode': 1, 'aReqRaw': 1., 'aReqValue': 1.})
      assert not native.safety_tx_hook(packet(denied))
      fixture.button = 2
      for _ in range(2):
        tick()
      fixture.button = 0
      for _ in range(2):
        tick()
      assert native.get_controls_allowed()
      assert native.safety_tx_hook(packet(denied))
      _, active = fixture.ci.apply(cc.as_reader(), fixture.ns)
      for frame in active:
        assert native.safety_tx_hook(packet(frame)), hex(frame[0])
      if withdrawal == 'cancel':
        fixture.button = 4
      elif withdrawal == 'main':
        fixture.main = True
      else:
        fixture.brake = True
      for _ in range(2):
        tick()
      assert not native.get_controls_allowed()
      assert not native.safety_tx_hook(packet(denied))
    finally:
      fixture.owner.close()
