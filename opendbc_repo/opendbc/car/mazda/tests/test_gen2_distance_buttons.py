"""CX-30 (GEN2) distance buttons: the closer button is the Wheel Controls distance button only at the closest setting."""
import pytest

from opendbc.can import CANPacker
from opendbc.car import gen_empty_fingerprint, structs
from opendbc.car.mazda.interface import CarInterface
from opendbc.car.mazda.values import CAR

GAP = structs.CarState.ButtonEvent.Type.gapAdjustCruise
CANCEL = structs.CarState.ButtonEvent.Type.cancel


def cx30(experience=0):
  cp = CarInterface.get_params(CAR.MAZDA_CX_30, gen_empty_fingerprint(), [], False, False, False)
  cp.alternativeExperience = experience
  return cp


def run(samples, experience=0):
  """samples: (distance setting, closer held, farther held[, cancel held]) per 10 ms frame."""
  ci = CarInterface(cx30(experience))
  packer = CANPacker('mazda_2019')
  events = []
  for tick, (setting, closer, farther, *cancel) in enumerate(samples, 1):
    frames = [packer.make_can_msg('CRUZE_STATE', 0, {'CRZ_STATE': 2 if setting else 0, 'DISTANCE_SETTING': setting}),
              packer.make_can_msg('CRZ_BTNS', 0, {'DISTANCE_LESS': closer, 'DISTANCE_MORE': farther,
                                                  'CAN': cancel[0] if cancel else 0})]
    state = ci.update([(tick * 10_000_000, frames)])
    events.extend((e.type, bool(e.pressed)) for e in state.buttonEvents)
  return events


def press(setting, closer=1, farther=0):
  return [(setting, 0, 0), (setting, closer, farther), (setting, closer, farther), (setting, 0, 0)]


@pytest.mark.parametrize('signal, pressed', (
  # Real CX-30 CRZ_BTNS frames, route 00000002--2702c05563: segment 21 (closer) and 22 (farther).
  ('DISTANCE_LESS', "c1 00 c0 00 00 04 04 2e"),
  ('DISTANCE_MORE', "c0 80 c0 00 00 04 0c b5"),
))
def test_dbc_bits_match_the_logged_frames(signal, pressed):
  idle = bytes.fromhex("c0 00 c0 00 00 04 06 2f")
  # Bytes 6-7 are the counter and checksum; the press is the only other difference from the idle frame.
  logged = bytes(a ^ b for a, b in zip(bytes.fromhex(pressed)[:6], idle[:6], strict=True))
  packed = CANPacker('mazda_2019').make_can_msg('CRZ_BTNS', 0, {signal: 1})[1][:6]
  assert logged == packed and bin(int.from_bytes(packed, 'big')).count('1') == 1


@pytest.mark.parametrize('experience', (0, 32))
def test_closer_press_at_the_closest_setting_is_the_distance_button(experience):
  assert run(press(1), experience) == [(GAP, True), (GAP, False)]


@pytest.mark.parametrize('setting', (0, 2, 3, 4))
def test_closer_press_anywhere_else_is_the_cars_own(setting):
  assert run(press(setting)) == []


@pytest.mark.parametrize('setting', (1, 2, 3, 4))
def test_farther_press_is_never_a_wheel_press(setting):
  assert run(press(setting, closer=0, farther=1)) == []


def test_a_press_that_lands_on_the_closest_setting_stays_the_cars_own():
  # The car steps 2 -> 1 while the press is still held; neither its press nor its release is reported.
  landing = [(2, 0, 0), (2, 1, 0), (1, 1, 0), (1, 0, 0)]
  assert run(landing) == []
  assert run(landing + press(1)) == [(GAP, True), (GAP, False)]


def test_cancel_and_distance_report_together_for_the_marked_profile():
  samples = [(1, 0, 0, 0), (1, 1, 0, 1), (1, 0, 0, 0)]
  assert sorted(run(samples, 32)) == sorted([(CANCEL, True), (GAP, True), (CANCEL, False), (GAP, False)])
  assert run(samples, 0) == [(GAP, True), (GAP, False)]
