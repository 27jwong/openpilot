from unittest.mock import patch

import pytest

from opendbc.can import CANPacker
from opendbc.car import Bus, structs
from opendbc.car.hyundai.interface import CarInterface
from opendbc.car.hyundai.ioniq5pe_long_profile import candidate
from opendbc.car.hyundai.ioniq5pe_startup import PEPendingStartup, Phase
from opendbc.car.hyundai.ioniq6_handoff import TimestampedCanPacket
from opendbc.car.hyundai.hyundaicanfd import hkg_can_fd_checksum
from opendbc.car.hyundai.tests.test_ioniq5pe_pending_startup import ThreadFixture
from opendbc.car.hyundai.tests.test_ioniq5pe_stock import params
from opendbc.car.hyundai.tests.test_ev9_long_controller import packets
from opendbc.car.hyundai.values import CAR, DBC


class ParkedFixture:
  def __init__(self):
    self.ns = 1_000_000_000
    self.tick = 0
    self.packer = CANPacker(DBC[CAR.HYUNDAI_IONIQ_5_PE][Bus.pt])
    self.queries = []
    self.sent = []
    self.gear = 0
    self.moving = False
    self.ready = True
    self.cruise = False
    self.button = 0
    self.main = False
    self.brake = False
    self.stock_scc = True
    self.quiet = False
    self.disable_ok = True
    self.restore_ok = True
    self.drop = None
    self.stock = params()
    self.owner = PEPendingStartup(candidate(self.stock, enabled=True, is_release=False),
                                  (self.recv, self.sent.extend), stock_cp=self.stock,
                                  clock=lambda: self.ns / 1e9, boot_clock=lambda: self.ns,
                                  exchange=self.exchange, thread_factory=ThreadFixture)

  def frames(self):
    frames = packets(self.packer, self.tick, advance_counter=True, gear=self.gear, moving=self.moving,
                     cruise=self.cruise, cruise_button=self.button, adaptive_main=self.main)
    result = []
    for address, data, bus in frames:
      if address == 0x35:
        raw = bytearray(data)
        if self.ready:
          raw[3] |= 0x40
        raw[:2] = hkg_can_fd_checksum(address, None, raw).to_bytes(2, 'little')
        data = bytes(raw)
      if address == 0x175 and self.brake:
        _, data, _ = self.packer.make_can_msg('TCS', 1, {'COUNTER': self.tick // 2, 'DriverBraking': 1})
      if address == 0x362:
        _, data, _ = self.packer.make_can_msg('CAM_0x362', 2, {'COUNTER': self.tick // 5})
      result.append((address, data, bus))
    if self.stock_scc and not self.quiet and self.tick % 2 == 0:
      result.append(self.packer.make_can_msg('SCC_CONTROL', 1, {'COUNTER': self.tick // 2, 'ACCMode': int(self.cruise)}))
    return [frame for frame in result if frame[0] != self.drop]

  def recv(self, **kwargs):
    self.tick += 1
    self.ns += 10_000_000
    return [TimestampedCanPacket(self.frames(), self.ns)]

  def exchange(self, recv, send, request, response):
    self.queries.append((request, response))
    if request == b'\x28\x01\x01':
      if self.disable_ok:
        self.quiet = True
      return self.disable_ok
    if request == b'\x28\x00\x01':
      return self.restore_ok
    return True

  def prepare(self):
    cp = self.owner.prepare(admission=lambda: True)
    self.ci = CarInterface(cp)
    self.owner.configure(self.ci)
    self.owner.finalize_aol_configuration(self.ci)
    self.owner.seal_publication()
    return cp

  def warm(self, ticks=15):
    for _ in range(ticks):
      packet = self.recv()[0]
      self.ci.update([(packet.log_mono_time_ns, list(packet))])

  def commit(self):
    self.owner.commit_pending(self.ci, sources_current=self.owner.sources_current(self.ci, self.ns), control_current=True)


@pytest.fixture
def parked():
  value = ParkedFixture()
  with patch('time.clock_gettime_ns', side_effect=lambda clock: value.ns):
    yield value
  value.owner.close()


def test_ready_parked_publication_is_pending_and_sends_nothing_before_initialized_commit(parked):
  stock = parked.stock.to_dict()
  cp = parked.prepare()
  assert cp.openpilotLongitudinalControl and cp.safetyConfigs[0].safetyParam == 0x5495
  assert parked.owner.parked_ready is not None and parked.owner.pending_target
  assert parked.owner.owner.phase is Phase.PENDING and not parked.owner.owner.active()
  assert not parked.queries and not parked.sent and parked.owner.thread is None
  assert parked.stock.to_dict() == stock
  parked.warm()
  assert parked.owner.parked_ready.matches_target(parked.ci, parked.ns)
  cc = structs.CarControl(enabled=True, latActive=True, longActive=True)
  assert parked.ci.apply(cc.as_reader(), parked.ns)[1] == []
  parked.commit()
  assert parked.owner.owner.active() and parked.owner.thread.started
  assert parked.queries[:2] == [(b'\x10\x03', b'\x50\x03'), (b'\x28\x01\x01', b'\x68\x01')]
  assert not parked.owner.can_emit(parked.ci.CC) and not parked.owner.handed_off
  assert not parked.owner.sources_current(parked.ci, parked.ns)


@pytest.mark.parametrize('failure', ['drive', 'reverse', 'neutral', 'moving', 'not_ready', 'cruise', 'cancel', 'main',
                                     'missing_scc', 'missing_cam', 'missing_mdps', 'missing_accel', 'missing_wheels',
                                     'missing_tcs', 'missing_steering', 'missing_buttons', 'missing_doors'])
def test_unproven_ready_start_preserves_exact_stock_and_never_disables(parked, failure):
  if failure in ('drive', 'reverse', 'neutral'):
    parked.gear = {'drive': 5, 'reverse': 7, 'neutral': 6}[failure]
  elif failure == 'moving':
    parked.moving = True
  elif failure == 'not_ready':
    # The existing READY=false path remains separate and unchanged.
    with patch.object(parked.owner, '_ready_probe', return_value=None):
      parked.prepare()
    assert parked.owner.owner.phase is Phase.STOCK
    return
  elif failure == 'cruise':
    parked.cruise = True
  elif failure == 'cancel':
    parked.button = 4
  elif failure == 'main':
    parked.main = True
  else:
    parked.drop = {'missing_scc': 0x1A0, 'missing_cam': 0x362, 'missing_mdps': 0xEA, 'missing_accel': 0x35,
                   'missing_wheels': 0xA0, 'missing_tcs': 0x175, 'missing_steering': 0x125,
                   'missing_buttons': 0x1CF, 'missing_doors': 0x411}[failure]
  cp = parked.prepare()
  assert cp.to_dict() == parked.stock.to_dict()
  assert parked.owner.owner.phase is Phase.STOCK and not parked.owner.pending_target
  assert not parked.queries and not parked.sent


@pytest.mark.parametrize('failure', ['drive', 'reverse', 'neutral', 'moving', 'not_ready', 'cruise', 'cancel', 'main',
                                     'stale', 'lost_proof', 'target_stamp', 'gas', 'door', 'seatbelt', 'fault',
                                     'bad_counter', 'cp_mutation', 'reordered', 'future', 'zero_stamp'])
def test_lost_parked_proof_before_commit_aborts_without_ecu_mutation(parked, failure):
  parked.prepare()
  parked.warm()
  assert parked.owner.parked_ready.matches_target(parked.ci, parked.ns)
  if failure in ('drive', 'reverse', 'neutral'):
    parked.gear = {'drive': 5, 'reverse': 7, 'neutral': 6}[failure]
    parked.warm()
  elif failure in ('moving', 'cruise'):
    setattr(parked, failure, True)
    parked.warm()
  elif failure == 'not_ready':
    parked.ready = False
    parked.warm()
  elif failure in ('cancel', 'main'):
    parked.button, parked.main = (4, False) if failure == 'cancel' else (0, True)
    parked.warm()
  else:
    proof = parked.owner.parked_ready
    if failure == 'stale':
      parked.ns += 300_000_001
    elif failure == 'lost_proof':
      proof.invalid = True
    elif failure == 'target_stamp':
      source = parked.ci.can_parsers[Bus.pt].message_states[0x35]
      source.timestamps[-1] -= 1
    elif failure in ('gas', 'door', 'seatbelt', 'fault'):
      setattr(proof.ci.CS.out, {'gas': 'gasPressed', 'door': 'doorOpen', 'seatbelt': 'seatbeltUnlatched',
                              'fault': 'steerFaultTemporary'}[failure], True)
    elif failure == 'bad_counter':
      from opendbc.can.parser import MAX_BAD_COUNTER
      proof.ci.can_parsers[Bus.pt].message_states[0x35].counter_fail = MAX_BAD_COUNTER
    elif failure == 'cp_mutation':
      proof.ci.CP.pcmCruise = False
    elif failure == 'reordered':
      proof.observe([(proof.last_stamp, [])], parked.ns)
    elif failure == 'future':
      proof.observe([(parked.ns + 1, [])], parked.ns)
    elif failure == 'zero_stamp':
      proof.observe([(0, [])], parked.ns)
  with pytest.raises(RuntimeError):
    parked.commit()
  assert not parked.queries and not parked.owner.session_attempted and not parked.owner.takeover_attempted
  assert not parked.owner.can_emit(parked.ci.CC)


@pytest.mark.parametrize('failure', ['disable_negative', 'quiet_collision', 'movement_during_query', 'ready_loss_during_query'])
def test_acknowledgement_quiet_and_parked_continuity_are_required(parked, failure):
  parked.prepare()
  parked.warm()
  original = parked.owner.exchange

  def exchange(*args):
    result = original(*args)
    if args[2] == b'\x28\x01\x01':
      if failure == 'quiet_collision':
        parked.quiet = False
      elif failure == 'movement_during_query':
        parked.gear, parked.moving = 5, True
      elif failure == 'ready_loss_during_query':
        parked.ready = False
    return result
  parked.owner.exchange = exchange
  parked.disable_ok = failure != 'disable_negative'
  before = parked.ci.CP.to_dict()
  with pytest.raises(RuntimeError):
    parked.commit()
  assert parked.owner.takeover_attempted and parked.owner.restored
  assert parked.owner.owner.phase is Phase.RESTORED
  assert parked.ci.CP.to_dict() == before
  assert not parked.owner.can_emit(parked.ci.CC)
