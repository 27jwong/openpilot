"""The first MAIN-on edge after boot lands on a frame card still reports as faulted.

TransportPauseFeedback starts failed and stays failed until its first acknowledged axis reply, and the CX-30
boots with MAIN off, so the only reply before the first MAIN edge is the unacknowledged MAIN-off one. On real
drives the edge frame had fault_active=True, the next frame False, and the MAIN pulse was spent on the faulted
frame. These tests drive the same frame sequence through the real intent.
"""

import pytest
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR
from openpilot.cereal import messaging
from openpilot.starpilot.aol.intent import AolSettings
from openpilot.starpilot.aol.tests.test_gm_held_native import Peers
from openpilot.starpilot.aol.wire import PandaSlot, SafetyState, encode_safety
from openpilot.starpilot.car.mazda.stock_intent import MAIN_EDGE_HOLD_NS, MazdaStockCardIntent

NOW = 10_000_000_000
FRAME = 10_000_000
PRODUCER = 'mazda-card'
SESSION = 'mazda-axis'
MAIN_OFF = (('wrongCarMode', 'user'), ('pcmDisable', 'user'))


def cx30():
  cp = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False)
  cp.alternativeExperience = 32
  return cp


def car_state(available, *, brake=False):
  cs = structs.CarState(canValid=True, canTimeout=False, vEgo=10.0, brakePressed=brake,
                        gearShifter=structs.CarState.GearShifter.drive)
  cs.cruiseState.available = available
  return cs.as_reader()


def peers(cp, now, *, events=MAIN_OFF, ack=False, mode='off', producer=PRODUCER, requested=False):
  """What card holds at a MAIN edge: the MAIN-off axis reply, its events, and a healthy idle panda."""
  cfg = cp.safetyConfigs[0]
  observed = now - 30_000_000
  slot = PandaSlot(0, 'mazda-panda', cfg.safetyModel.raw, cfg.safetyParam, 32, False)
  native = SafetyState(1, True, observed, observed + 200_000_000, cfg.safetyModel.raw, cfg.safetyParam,
                       False, False, requested, False, 'mazda-panda', SESSION,
                       sourcePandaStatesMonoTime=observed, pandaInventory=(slot,))
  wire = messaging.new_message('aolSafetyWire', 0, valid=True, logMonoTime=observed)
  wire.aolSafetyWire = encode_safety(native)
  stamp = now - 5_000_000
  axis = messaging.new_message('aolAxisState', valid=True, logMonoTime=stamp)
  a = axis.aolAxisState
  a.sessionId = a.faultSessionId = SESSION
  a.sequence, a.qualified, a.faultReason, a.mode = 7, True, 'none', mode
  a.observedMonoTime, a.sourceCarStateMonoTime, a.validUntilMonoTime = stamp, now - 10_000_000, stamp + 30_000_000
  a.intentProducerSessionId, a.intentIntegrityEpoch = producer, 1
  a.faultEventMonoTime = now - 50_000_000
  a.nativeAcknowledged = ack
  panda = messaging.new_message('pandaStates', 1, valid=True, logMonoTime=observed)
  panda.pandaStates[0].safetyModel, panda.pandaStates[0].safetyParam = cfg.safetyModel, cfg.safetyParam
  panda.pandaStates[0].alternativeExperience = 32
  onroad = messaging.new_message('onroadEvents', len(events), valid=True, logMonoTime=now - 50_000_000)
  for event, (name, kind) in zip(onroad.onroadEvents, events, strict=True):
    event.name = name
    event.userDisable, event.softDisable, event.immediateDisable = kind == 'user', kind == 'soft', kind == 'immediate'
  return Peers([wire.as_reader(), axis.as_reader(), panda.as_reader(), onroad.as_reader()]), native


def frame(intent, cp, now, available, fault_active, **kwargs):
  brake = kwargs.pop('brake', False)
  cs = car_state(available, brake=brake)
  sm, native = peers(cp, now, **kwargs)
  intent.observe_startup_readiness(sm, cs, native=native, panda_ready=True, now_ns=now, producer=PRODUCER)
  intent.update(cs, fault_active=fault_active, now_ns=now)
  return intent.allowed_latch


def booted_with_main_off(cp):
  intent = MazdaStockCardIntent(cp, AolSettings(True, 0., 0, 0, (0, 0, 0), (0, 0, 0)))
  # Transport feedback has no baseline yet, so every MAIN-off frame before the first edge is faulted.
  for tick in range(5, 0, -1):
    assert not frame(intent, cp, NOW - tick * FRAME, False, True)
  return intent


def test_first_main_edge_arms_once_the_main_off_fault_clears():
  cp = cx30()
  intent = booted_with_main_off(cp)
  assert not frame(intent, cp, NOW, True, True)  # the edge frame itself: still the MAIN-off reply
  assert not frame(intent, cp, NOW + FRAME, True, False)  # first clean frame re-seats gesture neutral
  assert frame(intent, cp, NOW + 2 * FRAME, True, None)


def test_main_edge_on_a_clean_frame_still_arms_on_that_frame():
  cp = cx30()
  intent = MazdaStockCardIntent(cp, AolSettings(True, 0., 0, 0, (0, 0, 0), (0, 0, 0)))
  for tick in range(5, 0, -1):
    frame(intent, cp, NOW - tick * FRAME, False, None)
  assert frame(intent, cp, NOW, True, None)


@pytest.mark.parametrize('bad', ['immediate', 'soft', 'other_user_disable', 'no_main_off_event', 'acknowledged',
                                 'active_axis', 'producer', 'native_request'])
def test_only_the_healthy_main_off_reply_excuses_the_edge(bad):
  cp = cx30()
  intent = booted_with_main_off(cp)
  edge = {
    'immediate': {'events': (*MAIN_OFF, ('controlsMismatch', 'immediate'))},
    'soft': {'events': (*MAIN_OFF, ('commIssue', 'soft'))},
    'other_user_disable': {'events': (*MAIN_OFF, ('seatbeltNotLatched', 'user'))},
    'no_main_off_event': {'events': (('pcmDisable', 'user'),)},
    'acknowledged': {'ack': True},
    'active_axis': {'mode': 'lateralOnly'},
    'producer': {'producer': 'other-card'},
    'native_request': {'requested': True},
  }[bad]
  assert not frame(intent, cp, NOW, True, True, **edge)
  for tick in range(1, 6):
    assert not frame(intent, cp, NOW + tick * FRAME, True, False if tick % 2 else None)


@pytest.mark.parametrize('brake', [False, True])
def test_pedal_pressed_is_main_off_only_with_the_brake_down(brake):
  cp = cx30()
  intent = booted_with_main_off(cp)
  events = (*MAIN_OFF, ('pedalPressed', 'user'))
  assert not frame(intent, cp, NOW, True, True, events=events, brake=brake)
  frame(intent, cp, NOW + FRAME, True, False, brake=brake)
  assert frame(intent, cp, NOW + 2 * FRAME, True, None, brake=brake) is brake


def test_held_edge_expires_without_renewal():
  cp = cx30()
  intent = booted_with_main_off(cp)
  assert not frame(intent, cp, NOW, True, True)
  now = NOW
  while now <= NOW + MAIN_EDGE_HOLD_NS:
    now += FRAME
    assert not frame(intent, cp, now, True, True)
  for tick in range(1, 4):
    assert not frame(intent, cp, now + tick * FRAME, True, False)


@pytest.mark.parametrize('withdrawal', ['main_off', 'cancel', 'disabled'])
def test_held_edge_is_dropped_by_a_withdrawal(withdrawal):
  cp = cx30()
  intent = booted_with_main_off(cp)
  assert not frame(intent, cp, NOW, True, True)
  cs = car_state(withdrawal != 'main_off')
  if withdrawal == 'cancel':
    cs = cs.as_builder()
    cs.buttonEvents = [structs.CarState.ButtonEvent(type=structs.CarState.ButtonEvent.Type.cancel, pressed=True)]
    cs = cs.as_reader()
  elif withdrawal == 'disabled':
    intent.settings = AolSettings(False, 0., 0, 0, (0, 0, 0), (0, 0, 0))
  sm, native = peers(cp, NOW + FRAME)
  intent.observe_startup_readiness(sm, cs, native=native, panda_ready=True, now_ns=NOW + FRAME, producer=PRODUCER)
  intent.update(cs, fault_active=False, now_ns=NOW + FRAME)
  assert not intent.main_pending_ns and not intent.allowed_latch
  if withdrawal == 'main_off':
    return  # MAIN back on later is a new physical edge with its own arm, not this held one
  if withdrawal == 'disabled':
    intent.settings = AolSettings(True, 0., 0, 0, (0, 0, 0), (0, 0, 0))
  for tick in range(2, 6):
    assert not frame(intent, cp, NOW + tick * FRAME, True, None)
