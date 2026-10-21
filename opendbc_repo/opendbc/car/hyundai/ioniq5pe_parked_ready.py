from copy import deepcopy
from math import isfinite

from opendbc.can.parser import MAX_BAD_COUNTER
from opendbc.car import Bus, structs
from opendbc.car.hyundai.ccnc_ev_stock import qualified as stock_qualified
from opendbc.car.hyundai.ioniq5pe_ready import PEReadySource
from opendbc.car.hyundai.values import CAR


class PEParkedReadyProof:
  def __init__(self, stock_cp, floor_ns):
    if (stock_cp.carFingerprint != CAR.HYUNDAI_IONIQ_5_PE or not stock_qualified(stock_cp) or
        not stock_cp.alphaLongitudinalAvailable or floor_ns <= 0):
      raise ValueError('Exact unmarked PE stock configuration and source floor required')
    from opendbc.car.hyundai.interface import CarInterface
    self.ci = CarInterface(stock_cp.as_reader().as_builder())
    self.snapshot = deepcopy(self.ci.CP.to_dict())
    self.floor_ns = floor_ns
    self.last_stamp = 0
    self.invalid = False
    self.ready = PEReadySource()

  def observe(self, packets, now_ns):
    for stamp, frames in packets:
      if type(stamp) is not int or stamp <= 0 or stamp > now_ns:
        self.invalid = True
        continue
      if stamp <= self.floor_ns:
        if self.last_stamp:
          self.invalid = True
        continue
      if stamp <= self.last_stamp:
        self.invalid = True
        continue
      self.last_stamp = stamp
      self.ready.update([(stamp, frames)], now_ns)
      self.ci.update([(stamp, frames)])

  def current(self, now_ns):
    state = self.ci.CS.out
    if (self.invalid or self.ci.CP.to_dict() != self.snapshot or not state.canValid or state.canTimeout or
        state.gearShifter != structs.CarState.GearShifter.park or not state.standstill or
        not all(isfinite(value) for value in (state.vEgo, state.vEgoRaw)) or
        state.gasPressed or state.regenBraking or state.steerFaultTemporary or state.steerFaultPermanent or state.accFaulted or
        state.doorOpen or state.seatbeltUnlatched or state.cruiseState.enabled or not state.cruiseState.available or
        self.ready.current(now_ns, self.floor_ns, 100_000_000) is not True):
      return False
    parser = self.ci.can_parsers[Bus.pt]
    camera = self.ci.can_parsers[Bus.cam]
    if parser.bus != 1 or camera.bus != 2 or not camera.can_valid or camera.bus_timeout:
      return False
    names = ('MDPS', 'ACCELERATOR', 'WHEEL_SPEEDS', 'TCS', 'STEERING_SENSORS', 'CRUISE_BUTTONS',
             'SCC_CONTROL', 'DOORS_SEATBELTS')
    for source_parser, name in [*((parser, name) for name in names), (camera, 'CAM_0x362')]:
      source = source_parser.message_states.get(source_parser.dbc.name_to_msg[name].address)
      if (source is None or not source.timestamps or source.counter_fail >= MAX_BAD_COUNTER or
          not self.floor_ns < int(source.timestamps[-1]) <= now_ns or
          now_ns - int(source.timestamps[-1]) > source.timeout_threshold):
        return False
    buttons = parser.vl['CRUISE_BUTTONS']
    return all(buttons[name] == 0 for name in ('CRUISE_BUTTONS', 'ADAPTIVE_CRUISE_MAIN_BTN', 'NORMAL_CRUISE_MAIN_BTN'))

  def matches_target(self, ci, now_ns):
    if not self.current(now_ns) or ci.CP.carFingerprint != CAR.HYUNDAI_IONIQ_5_PE:
      return False
    stock, target = self.ci.can_parsers[Bus.pt], ci.can_parsers[Bus.pt]
    if target.bus != stock.bus or not ci.CS.out.canValid or ci.CS.out.canTimeout:
      return False
    for name in ('MDPS', 'ACCELERATOR', 'WHEEL_SPEEDS', 'TCS', 'STEERING_SENSORS', 'CRUISE_BUTTONS'):
      address = stock.dbc.name_to_msg[name].address
      a, b = stock.message_states.get(address), target.message_states.get(address)
      if a is None or b is None or not a.timestamps or not b.timestamps or a.timestamps[-1] != b.timestamps[-1]:
        return False
    return bool(ci.CS.out.gearShifter == structs.CarState.GearShifter.park and ci.CS.out.standstill and
                not ci.CS.out.gasPressed and not ci.CS.out.regenBraking)
