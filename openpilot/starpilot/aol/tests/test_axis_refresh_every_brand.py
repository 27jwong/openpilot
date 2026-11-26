"""Every brand receives a replacement proof at the authority check, not only GM.

The proof selfdrived publishes every 10 ms is valid for 30 ms, so any 31+ ms stall in
either process expires it. Recorded on a Mazda CX-30 (combined AOL + openpilot long):
  queued:     Controls paused 35 ms inside its step and found its proof 1 ms expired,
              with three newer proofs already queued (newest logged 1.5 ms before the deadline).
  publishing: selfdrived's proof gap was 43 ms; the proof had expired 2.8 ms before Controls
              woke, and the next one landed 3.3 ms after the check.
Both dropped steering and long for one frame. The refresh may only replace the proof:
neither the old nor the received proof gains time, and every gate runs on the new snapshot.
"""
from contextlib import ExitStack
from unittest.mock import patch

import pytest

from opendbc.car import gen_empty_fingerprint
from openpilot.cereal import messaging
from openpilot.common.prefix import OpenpilotPrefix
from openpilot.starpilot.aol.tests import test_ioniq_availability as fixture


def configuration(profile):
  long = profile.endswith('-long')
  if profile.startswith('sonata'):
    from opendbc.car.hyundai.interface import CarInterface
    from opendbc.car.hyundai.values import CAR
    cp = CarInterface.get_params(CAR.HYUNDAI_SONATA, gen_empty_fingerprint(), [], long, False, False)
  else:
    from opendbc.car.mazda.interface import CarInterface
    from opendbc.car.mazda.values import CAR
    if not hasattr(CAR, 'MAZDA_CX_30'):
      pytest.skip('no CX-30 on this stack')
    cp = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], True, False, False)
  if cp.openpilotLongitudinalControl != long:
    pytest.skip(f'{profile}: openpilot longitudinal is {cp.openpilotLongitudinalControl} on this stack')
  return cp


@pytest.mark.parametrize('profile', ['sonata-long', 'cx30-long', 'sonata-lateral'])
@pytest.mark.parametrize('delivery', ['queued', 'publishing'])
@pytest.mark.parametrize('arrival', ['healthy', 'empty', 'withdrawn', 'stale_axis', 'can_invalid', 'native_invalid'])
def test_expired_proof_is_replaced_at_the_authority_check_without_extending_it(profile, delivery, arrival):
  combined = profile.endswith('-long')
  case = fixture.IoniqAvailabilityTests(methodName='runTest')
  with ExitStack() as cleanup:
    cleanup.callback(case.doCleanups)
    cleanup.enter_context(OpenpilotPrefix())
    # Engaged in every case: with stock cruise the AOL proof is lateral-only
    _, controls, step = case._authority_callers(True, cp=configuration(profile))
    controls.lane_centering_host = None
    _, _, initial = step(0)
    assert initial.latActive and initial.longActive == combined
    axis = controls.sm['aolAxisState']
    deadline = int(axis.validUntilMonoTime)
    if delivery == 'queued':
      clock = [deadline - 34_000_000]  # fresh on entry
      gate = deadline + 1_000_000
      receipt = deadline - 1_500_000
    else:
      clock = [deadline + 2_800_000]  # already expired on entry, next proof not yet sent
      gate = deadline + 5_000_000
      receipt = deadline + 13_400_000
    queued = []
    for service in ('carState', 'aolAxisState', 'selfdriveState'):
      event = messaging.new_message(service, valid=True, logMonoTime=receipt)
      setattr(event, service, controls.sm[service])
      if service == 'carState':
        event.carState.canValid = arrival != 'can_invalid'
      elif service == 'aolAxisState':
        fresh = event.aolAxisState
        fresh.sequence += 1
        fresh.sourceCarStateMonoTime = fresh.observedMonoTime = receipt
        fresh.validUntilMonoTime = deadline if arrival == 'stale_axis' else receipt + 30_000_000
        fresh.lateralActive = fresh.desiredLateral = arrival != 'withdrawn'
        fresh.longitudinalActive = fresh.desiredLongitudinal = combined and arrival != 'withdrawn'
        fresh.qualified = arrival != 'can_invalid'
      queued.append(event.as_reader())

    def qualify(active):
      clock[0] = gate
      return active

    receives = []

    def receive(timeout):
      receives.append(timeout)
      if delivery == 'publishing' and len(receives) == 1:
        return  # nothing sent yet: the refresh has to wait for it
      if arrival != 'empty':
        controls.sm.update_msgs(receipt / 1e9, queued)
      if arrival == 'native_invalid':
        controls.sm.valid['aolSafetyWire'] = False

    def wait(timeout):
      clock[0] = max(clock[0], receipt) + 300_000

    cleanup.enter_context(patch.dict('os.environ', {'REPLAY': '0'}))
    cleanup.enter_context(patch('time.monotonic_ns', side_effect=lambda: clock[0]))
    cleanup.enter_context(patch.object(controls.longitudinal_inputs, 'qualify_active', side_effect=qualify))
    receiver = cleanup.enter_context(patch.object(controls.sm, 'update', side_effect=receive))
    poller = cleanup.enter_context(patch('openpilot.cereal.messaging.Poller'))
    poller.return_value.poll.side_effect = wait
    command, _ = controls.state_control()

    assert command.latActive == (arrival == 'healthy')
    assert command.longActive == (combined and arrival == 'healthy')
    waited = delivery == 'publishing' or arrival == 'empty'
    assert receiver.call_args_list == [((0,),)] * (2 if waited else 1)
    if waited:
      poller.return_value.registerSocket.assert_called_once_with(controls.sm.sock['aolAxisState'])
      poller.return_value.poll.assert_called_once_with(10)
    else:
      poller.assert_not_called()
    # Neither the original nor the received proof acquires a longer lease.
    assert axis.validUntilMonoTime == deadline
    expected = deadline if arrival in ('empty', 'stale_axis') else receipt + 30_000_000
    assert controls.sm['aolAxisState'].validUntilMonoTime == expected
