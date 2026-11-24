"""Mazda Cancel on a step that runs past the axis lease; real SD/Controls, synthetic clocks and transport."""
from contextlib import ExitStack
from unittest import mock

import pytest
from opendbc.car import gen_empty_fingerprint
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR, MazdaFlags
from opendbc.car.structs import car
from openpilot.cereal import log
from openpilot.common.params import Params
from openpilot.common.prefix import OpenpilotPrefix
from openpilot.starpilot.aol.intent import read_settings
from openpilot.starpilot.aol.tests import test_ioniq_availability as fixture
from openpilot.starpilot.aol.tests.test_input_deadline_continuity import BASE, intent


@pytest.mark.parametrize('restriction', ['none', 'latch_off', 'native_denied', 'brake', 'expired_source'])
@pytest.mark.parametrize('identity', [CAR.MAZDA_CX5, CAR.MAZDA_CX5_2022, CAR.MAZDA_CX9_2021])
def test_cancel_on_late_step_keeps_fresh_retained_lateral(identity, restriction):
  with OpenpilotPrefix(), ExitStack() as cleanup:
    case = fixture.IoniqAvailabilityTests(methodName='runTest')
    cleanup.callback(case.doCleanups)
    saved = Params()
    saved.put_bool('AlwaysOnLateral', True, block=True)
    cp = CarInterface.get_params(identity, gen_empty_fingerprint(), [], False, False, False)
    cp.flags = int(MazdaFlags.GEN1)
    cp.safetyConfigs[0].safetyParam = 0
    cp.alternativeExperience = 32
    sd, controls, drive = case._authority_callers(True, cp=cp, genuine_events=True)
    sd.aol_settings = read_settings(saved)
    state = fixture.car_state()
    state.cruiseState.available = state.cruiseState.enabled = True
    options = {'native_request_long': False, 'long_allowed': False, 'panda_controls_allowed': True}
    drive(0, observed_state=state, **options)
    prior, _, command = drive(2, state_stamp=BASE, offset_ns=4_237_448,
      intent_message=intent(BASE, 3), observed_state=state, **options)
    assert prior.lateralActive and not prior.longitudinalActive and command.latActive and not command.longActive
    # The next step starts just past the 30 ms lease of the last published axis.
    now = BASE + 34_538_124
    source = now - (31_000_000 if restriction == 'expired_source' else 24_218_150)
    assert now - prior.validUntilMonoTime == 300_676
    with mock.patch('openpilot.selfdrive.controls.controlsd.time.monotonic_ns', return_value=now):
      expired, _ = controls.state_control()
    assert not expired.latActive and not expired.longActive
    pressed = fixture.car_state()
    # Stock cruise still reports engaged on the frame that carries the press.
    pressed.cruiseState.available = pressed.cruiseState.enabled = True
    pressed.brakePressed = restriction == 'brake'
    pressed.buttonEvents = [car.CarState.ButtonEvent(type='cancel', pressed=True)]
    axis, _, command = drive(5, offset_ns=4_538_124, state_stamp=source,
      intent_message=intent(source, 4, latched=restriction != 'latch_off'), observed_state=pressed,
      allowed=restriction != 'native_denied', **options)
    assert log.OnroadEvent.EventName.buttonCancel in sd.events.names
    assert not axis.longitudinalActive and not command.longActive and not sd.enabled
    if restriction == 'none':
      assert axis.sourceCarStateMonoTime == source
      assert axis.lateralActive and command.latActive
      assert not sd.aol_authority_lost and sd.aol_last_intent.allowedLatch
      assert log.OnroadEvent.EventName.controlsMismatch not in sd.events.names
    else:
      assert not axis.lateralActive and not command.latActive
    if restriction == 'brake':
      # Only Cancel is excused; a brake override on the same late step still withdraws through the lease guard.
      assert log.OnroadEvent.EventName.pedalPressed in sd.events.names
      assert sd.aol_authority_lost and log.OnroadEvent.EventName.controlsMismatch in sd.events.names
