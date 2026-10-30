from opendbc.car import structs
from opendbc.car.mazda.stock_aol import qualified
from openpilot.cereal import log
from openpilot.selfdrive.car.cruise import CRUISE_LONG_PRESS
from openpilot.starpilot.aol.intent import AolCardIntent, AOL_TOGGLE, ButtonType

# A MAIN edge held for the MAIN-off fault to clear arms within this window or not at all. Panda's
# own MAIN grant waits 300 ms for the host request that follows the arm.
MAIN_EDGE_HOLD_NS = 100_000_000
EventName = log.OnroadEvent.EventName


class MazdaStockCardIntent(AolCardIntent):
  observe_stock_engagement = True

  def __init__(self, cp, settings):
    super().__init__(settings, explicit_latch=True)
    self.cp = cp
    self.main_seen = False
    self.startup_main_pending = True
    self.startup_main_source_ns = 0
    self.startup_native_ready = False
    self.startup_binding = None
    self.main_available = False
    self.main_neutral = False
    self.main_pulse = False
    self.main_pending_ns = 0
    self.main_off_response_current = False
    self.cancel_held = False
    self.cancel_ticks = 0

  def _observe_main_off_response(self, sm, CS, *, native, panda_ready, now_ns, producer):
    """Qualify only the healthy MAIN-off reply still current at a new MAIN edge.

    Transport feedback stays failed from boot until its first acknowledged reply, and the
    CX-30 boots with MAIN off, so the first MAIN-on edge always lands on a faulted frame.
    """
    from openpilot.starpilot.aol.runtime import AXIS_MAX_AGE_NS
    from openpilot.starpilot.aol.transport_pause import _healthy_transport_native

    if not (self.main_seen and self.main_neutral and not self.main_available and CS.cruiseState.available):
      return False
    if (not qualified(self.cp, marked_only=True) or not panda_ready or not producer or
        not CS.canValid or CS.canTimeout or CS.steerFaultTemporary or CS.steerFaultPermanent or CS.accFaulted or
        not sm.seen['aolAxisState'] or not sm.valid['aolAxisState'] or not sm.alive['aolAxisState'] or
        not sm.seen['onroadEvents'] or not sm.valid['onroadEvents'] or not sm.alive['onroadEvents']):
      return False
    axis = sm['aolAxisState']
    stamp = int(sm.logMonoTime['aolAxisState'])
    if (not axis.qualified or not axis.sessionId or axis.faultSessionId != axis.sessionId or axis.sequence <= 0 or
        axis.intentProducerSessionId != producer or stamp != axis.observedMonoTime or
        not 0 < axis.sourceCarStateMonoTime <= stamp < now_ns <= axis.validUntilMonoTime or
        now_ns - axis.sourceCarStateMonoTime > AXIS_MAX_AGE_NS or axis.validUntilMonoTime - stamp > AXIS_MAX_AGE_NS or
        str(axis.faultReason) != 'none' or str(axis.mode) != 'off' or axis.nativeAcknowledged or
        axis.desiredLateral or axis.desiredLongitudinal or axis.lateralActive or axis.longitudinalActive):
      return False
    event_ns = int(sm.logMonoTime['onroadEvents'])
    if not 0 < event_ns <= stamp or now_ns - event_ns > 1_500_000_000:
      return False
    # Only the MAIN-off picture: wrongCarMode, its pcmDisable, and a held brake's pedalPressed. Any other
    # user, soft or immediate disable is a real reason not to arm.
    main_off = False
    for event in sm['onroadEvents']:
      if event.immediateDisable or event.softDisable:
        return False
      main_off |= event.name == EventName.wrongCarMode and event.userDisable
      if event.userDisable and not (event.name in (EventName.wrongCarMode, EventName.pcmDisable) or
                                    event.name == EventName.pedalPressed and CS.brakePressed):
        return False
    current = _healthy_transport_native(sm, self.cp, CS, str(axis.sessionId), now_ns)
    return bool(main_off and native is not None and current is not None and current.pandaInventory and
                not current.requestedLateral and not current.lateralAllowed and
                not current.requestedLongitudinal and not current.longitudinalAllowed)

  def observe_startup_readiness(self, sm, CS, *, native, panda_ready, now_ns, producer):
    from openpilot.starpilot.aol.intent import disarming_fault
    from openpilot.starpilot.aol.runtime import AXIS_MAX_AGE_NS
    from openpilot.starpilot.aol.transport_pause import _healthy_transport_native, availability_only

    try:
      self.main_off_response_current = self._observe_main_off_response(
        sm, CS, native=native, panda_ready=panda_ready, now_ns=now_ns, producer=producer)
    except (AttributeError, KeyError, IndexError, TypeError, ValueError, OverflowError):
      self.main_off_response_current = False
    self.startup_native_ready = False
    if not self.startup_main_pending or not qualified(self.cp, marked_only=True):
      return
    if CS.steerFaultPermanent or CS.accFaulted:
      self.startup_main_pending = False
      return
    try:
      event_ns = int(sm.logMonoTime['onroadEvents'])
      events_current = bool(sm.seen['onroadEvents'] and sm.valid['onroadEvents'] and sm.alive['onroadEvents'] and
        0 < event_ns <= now_ns and now_ns - event_ns <= 1_500_000_000)
      events = sm['onroadEvents']
      faults = [event for event in events if disarming_fault([event], CS)]
      if events_current and faults and not availability_only(faults):
        self.startup_main_pending = False
        return
      axis = sm['aolAxisState']
      native = (_healthy_transport_native(sm, self.cp, CS, str(axis.sessionId), now_ns, native)
                if native is not None else None)
      self.startup_native_ready = bool(events_current and panda_ready and native is not None and native.pandaInventory and
        not native.requestedLateral and not native.lateralAllowed and not native.requestedLongitudinal and not native.longitudinalAllowed and
        all(not slot.controlsAllowed for slot in native.pandaInventory) and
        sm.seen['aolAxisState'] and sm.valid['aolAxisState'] and sm.alive['aolAxisState'] and
        int(sm.logMonoTime['aolAxisState']) == axis.observedMonoTime and
        axis.validUntilMonoTime - axis.observedMonoTime <= AXIS_MAX_AGE_NS and
        axis.qualified and axis.nativeAcknowledged and str(axis.faultReason) == 'none' and
        str(axis.sessionId) == native.axisSessionId and producer and axis.intentProducerSessionId == producer and
        not axis.desiredLateral and not axis.desiredLongitudinal and not axis.lateralActive and not axis.longitudinalActive and
        0 < axis.sourceCarStateMonoTime <= axis.observedMonoTime <= now_ns <= axis.validUntilMonoTime and
        now_ns - axis.sourceCarStateMonoTime <= AXIS_MAX_AGE_NS)
    except (AttributeError, KeyError, TypeError, ValueError, OverflowError):
      self.startup_native_ready = False
    if self.startup_native_ready:
      binding = (str(axis.sessionId), producer, int(axis.intentIntegrityEpoch), native.pandaSerial,
                 tuple(slot.hardwareSerial for slot in native.pandaInventory))
      if binding != self.startup_binding:
        self.startup_main_source_ns = 0
        self.startup_binding = binding
    else:
      self.startup_binding = None
      self.startup_main_source_ns = 0

  def cancel_action(self, action):
    if action == AOL_TOGGLE and self.settings.enabled:
      self.allowed_latch = not self.allowed_latch
      if self.allowed_latch:
        self.pause_lateral = False
    else:
      self._perform(action)

  def update(self, CS, **kwargs):
    state = structs.CarState(**CS.to_dict())
    events = []
    for event in CS.buttonEvents:
      if event.type == ButtonType.cancel:
        if event.pressed:
          self.cancel_held = True
          self.cancel_ticks = 0
        else:
          if self.cancel_held and 0 < self.cancel_ticks < CRUISE_LONG_PRESS:
            self.cancel_action(self.settings.cancel_actions[0])
          self.cancel_held = False
          self.cancel_ticks = 0
      elif event.type in (ButtonType.mainCruise, ButtonType.lkas):
        action = self.settings.main_action if event.type == ButtonType.mainCruise else self.settings.lkas_action
        if action == AOL_TOGGLE:
          events.append(structs.CarState.ButtonEvent(**event.to_dict()))
        elif event.pressed:
          self._perform(action)
      else:
        events.append(structs.CarState.ButtonEvent(**event.to_dict()))
    available = bool(state.cruiseState.available)
    healthy = state.canValid and not state.canTimeout
    now_ns = kwargs.get('now_ns', 0)
    pending_ns, self.main_pending_ns = self.main_pending_ns, 0
    main_off_ok, self.main_off_response_current = self.main_off_response_current, False
    if healthy:
      if self.main_pulse:
        events.append(structs.CarState.ButtonEvent(type=ButtonType.mainCruise, pressed=False))
        self.main_pulse = False
      if not available:
        self.main_neutral = True
      elif self.main_seen and self.main_neutral and not self.main_available:
        self.main_neutral = False
        if kwargs.get('fault_active') is not True:
          events.append(structs.CarState.ButtonEvent(type=ButtonType.mainCruise, pressed=True))
          self.main_pulse = True
        elif main_off_ok and now_ns > 0:
          # Hold the edge until the MAIN-off fault clears and a clean frame re-seats gesture neutral.
          self.main_pending_ns = now_ns
      elif (pending_ns and 0 < now_ns - pending_ns <= MAIN_EDGE_HOLD_NS and self.settings.enabled and
            not self.cancel_held and not state.steerFaultPermanent and not state.accFaulted):
        if not self._fault_inhibit and self._gesture_neutral_seen:
          events.append(structs.CarState.ButtonEvent(type=ButtonType.mainCruise, pressed=True))
          self.main_pulse = True
        else:
          # The deadline stays bound to the physical edge; waiting never renews it.
          self.main_pending_ns = pending_ns
      self.main_seen = True
      self.main_available = available
    else:
      self.cancel_held = False
      self.cancel_ticks = 0
      self.main_neutral = False
    startup_main = False
    if self.startup_main_pending and healthy:
      eligible = bool(available and not state.cruiseState.enabled and self.settings.enabled and
        self.main_action != AOL_TOGGLE and self.lkas_action != AOL_TOGGLE and
        not state.steerFaultPermanent and not state.accFaulted and
        not (self.cancel_held and any(self.settings.cancel_actions)))
      source_ns = kwargs.get('now_ns', 0)
      if not eligible:
        self.startup_main_pending = False
      elif self.startup_native_ready and kwargs.get('fault_active'):
        self.startup_main_pending = False
      elif self.startup_native_ready and self.startup_main_source_ns and source_ns > self.startup_main_source_ns:
        # The first valid sample initializes the transport feedback pipeline.
        # Confirm MAIN on a subsequent fresh sample before admitting startup intent.
        self.startup_main_pending = False
        startup_main = True
      elif self.startup_native_ready and source_ns > 0:
        self.startup_main_source_ns = source_ns
    state.buttonEvents = events
    super().update(state.as_reader(), **kwargs)
    if startup_main and not self._fault_inhibit:
      self.allowed_latch = True
      self._last_latch_edge_ns = kwargs.get('now_ns', 0)
    if not available:
      self.allowed_latch = False
    if self.cancel_held:
      self.cancel_ticks += 1
      if self.cancel_ticks == CRUISE_LONG_PRESS:
        self.cancel_action(self.settings.cancel_actions[1])
      elif self.cancel_ticks == CRUISE_LONG_PRESS * 5:
        self.cancel_action(self.settings.cancel_actions[2])
