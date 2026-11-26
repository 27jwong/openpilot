"""Observable lane-change state sequences under optional saved preferences."""

from types import SimpleNamespace
import unittest

from openpilot.cereal import log
from openpilot.common.constants import CV
from openpilot.selfdrive.controls.lib.desire_helper import DesireHelper
from openpilot.starpilot.lateral.lane_change_preferences import LaneChangePolicy


def car(**changes):
  values = {"vEgo": 30 * CV.MPH_TO_MS, "leftBlinker": False, "rightBlinker": False,
            "steeringPressed": False, "steeringTorque": 0.0, "leftBlindspot": False, "rightBlindspot": False}
  values.update(changes)
  return SimpleNamespace(**values)


def observed(helper, state, active=True, probability=1.0):
  helper.update(state, active, probability)
  return helper.lane_change_state, helper.lane_change_direction, helper.desire


class DesireHelperPreferencesTests(unittest.TestCase):
  def test_auto_delay_geometry_and_manual_fallback(self):
    policy = LaneChangePolicy(True, 0.0, False, True, 0.10, 3.0)
    helper = DesireHelper(policy)
    left = car(leftBlinker=True)
    helper.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual((helper.lane_change_state, helper.desire), (log.LaneChangeState.preLaneChange, log.Desire.none))
    helper.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.preLaneChange)
    for _ in range(3):
      helper.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual((helper.lane_change_state, helper.desire),
                     (log.LaneChangeState.laneChangeStarting, log.Desire.laneChangeLeft))

    blocked = DesireHelper(policy)
    blocked.update(left, True, 1.0, auto_evidence=False, engaged=True)
    for _ in range(10):
      blocked.update(left, True, 1.0, auto_evidence=False, engaged=True)
    self.assertEqual(blocked.lane_change_state, log.LaneChangeState.preLaneChange)
    blocked.update(car(leftBlinker=True, steeringPressed=True, steeringTorque=1), True, 1.0,
                   auto_evidence=False, engaged=True)
    self.assertEqual(blocked.lane_change_state, log.LaneChangeState.laneChangeStarting)

    delayed = DesireHelper(policy)
    delayed.update(left, True, 1.0, auto_evidence=True, engaged=True)
    delayed.update(left, True, 1.0, auto_evidence=True, engaged=True)
    delayed.update(left, True, 1.0, auto_evidence=False, engaged=True)
    delayed.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(delayed.lane_change_state, log.LaneChangeState.preLaneChange)
    delayed.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(delayed.lane_change_state, log.LaneChangeState.laneChangeStarting)

    disengaged = DesireHelper(policy)
    disengaged.update(left, True, 1.0, auto_evidence=True, engaged=True)
    disengaged.update(left, True, 1.0, auto_evidence=True, engaged=False)
    self.assertEqual(disengaged.lane_change_state, log.LaneChangeState.preLaneChange)
    disengaged.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(disengaged.lane_change_state, log.LaneChangeState.preLaneChange)
    disengaged.update(car(leftBlinker=True, steeringPressed=True, steeringTorque=1), True, 1.0,
                       auto_evidence=False, engaged=False)
    self.assertEqual(disengaged.lane_change_state, log.LaneChangeState.laneChangeStarting)

  def test_auto_never_uses_blindspot_or_unengaged_and_hazards_disarm(self):
    policy = LaneChangePolicy(True, 0.0, True, True, 0.0, 3.0)
    helper = DesireHelper(policy)
    left = car(leftBlinker=True)
    helper.update(left, True, 1.0, auto_evidence=True, engaged=False)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.preLaneChange)
    helper.update(car(), True, 1.0, auto_evidence=True, engaged=True)
    helper.update(left, True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.preLaneChange)
    helper.update(car(leftBlinker=True, leftBlindspot=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.preLaneChange)
    helper.update(car(leftBlinker=True, rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.off)
    helper.update(car(rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.preLaneChange)
    self.assertEqual(helper.auto_status, "manualRequired")
    helper.update(car(), True, 1.0, auto_evidence=True, engaged=True)
    helper.update(car(rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    helper.update(car(rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual((helper.lane_change_state, helper.lane_change_direction, helper.desire),
                     (log.LaneChangeState.laneChangeStarting, log.LaneChangeDirection.right, log.Desire.laneChangeRight))
    for _ in range(30):
      helper.update(car(rightBlinker=True), True, 0.0, auto_evidence=True, engaged=True)
    self.assertEqual((helper.lane_change_state, helper.lane_change_direction, helper.desire),
                     (log.LaneChangeState.off, log.LaneChangeDirection.none, log.Desire.none))
    helper.update(car(leftBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(helper.lane_change_state, log.LaneChangeState.off)

    manual = DesireHelper(LaneChangePolicy(True, 0.0, False, True, 0.0, 3.0))
    manual.update(car(leftBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    manual.update(car(leftBlinker=True, rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    manual.update(car(rightBlinker=True), True, 1.0, auto_evidence=True, engaged=True)
    self.assertEqual(manual.lane_change_state, log.LaneChangeState.preLaneChange)
    manual.update(car(rightBlinker=True, steeringPressed=True, steeringTorque=-1), True, 1.0,
                  auto_evidence=False, engaged=True)
    self.assertEqual(manual.lane_change_state, log.LaneChangeState.laneChangeStarting)

  def test_absent_policy_keeps_stock_nudge_and_blindspot(self):
    helper = DesireHelper()
    self.assertEqual(observed(helper, car(leftBlinker=True)),
                     (log.LaneChangeState.preLaneChange, log.LaneChangeDirection.left, log.Desire.none))
    self.assertEqual(observed(helper, car(leftBlinker=True, steeringPressed=True, steeringTorque=1, leftBlindspot=True))[0],
                     log.LaneChangeState.preLaneChange)
    self.assertEqual(observed(helper, car(leftBlinker=True, steeringPressed=True, steeringTorque=1))[0],
                     log.LaneChangeState.laneChangeStarting)

  def test_disabled_and_speed_boundary(self):
    disabled = DesireHelper(LaneChangePolicy(False, 0.0, False))
    self.assertEqual(observed(disabled, car(leftBlinker=True, steeringPressed=True, steeringTorque=1)),
                     (log.LaneChangeState.off, log.LaneChangeDirection.none, log.Desire.none))
    minimum = 45 * CV.MPH_TO_MS
    helper = DesireHelper(LaneChangePolicy(True, minimum, False))
    self.assertEqual(observed(helper, car(leftBlinker=True, vEgo=minimum - 0.01))[0], log.LaneChangeState.off)
    observed(helper, car())
    self.assertEqual(observed(helper, car(leftBlinker=True, vEgo=minimum))[0], log.LaneChangeState.preLaneChange)

  def test_once_requires_both_blinkers_off_after_completion_and_interruption(self):
    helper = DesireHelper(LaneChangePolicy(True, 0.0, True))
    left = car(leftBlinker=True)
    nudged = car(leftBlinker=True, steeringPressed=True, steeringTorque=1)
    self.assertEqual(observed(helper, left)[0], log.LaneChangeState.preLaneChange)
    self.assertEqual(observed(helper, nudged),
                     (log.LaneChangeState.laneChangeStarting, log.LaneChangeDirection.left, log.Desire.laneChangeLeft))
    for _ in range(30):
      last = observed(helper, left, probability=0.0)
    self.assertEqual(last, (log.LaneChangeState.off, log.LaneChangeDirection.none, log.Desire.none))
    self.assertEqual(observed(helper, car(leftBlinker=True, rightBlinker=True))[0], log.LaneChangeState.off)
    self.assertEqual(observed(helper, car(rightBlinker=True, steeringPressed=True, steeringTorque=-1))[0], log.LaneChangeState.off)
    self.assertEqual(observed(helper, car(rightBlinker=True), active=False)[0], log.LaneChangeState.off)
    self.assertEqual(observed(helper, car(rightBlinker=True))[0], log.LaneChangeState.off)
    observed(helper, car())
    self.assertEqual(observed(helper, car(rightBlinker=True))[0], log.LaneChangeState.preLaneChange)

  def test_once_survives_timeout(self):
    helper = DesireHelper(LaneChangePolicy(True, 0.0, True))
    observed(helper, car(leftBlinker=True))
    observed(helper, car(leftBlinker=True, steeringPressed=True, steeringTorque=1))
    helper.lane_change_timer = 11.0
    self.assertEqual(observed(helper, car(leftBlinker=True))[0], log.LaneChangeState.off)
    self.assertEqual(observed(helper, car(rightBlinker=True))[0], log.LaneChangeState.off)
    observed(helper, car())
    self.assertEqual(observed(helper, car(rightBlinker=True))[0], log.LaneChangeState.preLaneChange)

  def test_one_frame_control_dropout_keeps_the_automatic_change_for_this_signal(self):
    # Mazda CX-30, 73 mph: controls dropped latActive for one 10 ms frame right after the
    # left signal went on, and the clear 3.2 m lane showed "Automatic Change Unavailable".
    policy = LaneChangePolicy(True, 0.0, True, True, 1.0, 3.0)
    left = car(leftBlinker=True, vEgo=73 * CV.MPH_TO_MS)

    def frame(helper, active=True, state=left):
      helper.update(state, active, 1.0, auto_evidence=True, engaged=active)
      return helper.lane_change_state, helper.auto_status

    def frames_until_automatic_start(helper):
      # the signal's first frame enters preLaneChange; each frame after waits out the auto delay
      self.assertEqual(frame(helper)[0], log.LaneChangeState.preLaneChange)
      for count in range(1, 30):
        state, status = frame(helper)
        if state == log.LaneChangeState.laneChangeStarting:
          return count
        self.assertEqual((state, status), (log.LaneChangeState.preLaneChange, "waitingForDelay"))
      return None

    blip = DesireHelper(policy)
    frame(blip)
    for _ in range(4):
      self.assertEqual(frame(blip), (log.LaneChangeState.preLaneChange, "waitingForDelay"))
    frame(blip, active=False)  # the model frame that sampled the dropout
    self.assertFalse(blip.auto_signal_blocked)
    self.assertIn(frames_until_automatic_start(blip), (19, 20, 21))  # the full 1 s delay again, not cancelled

    # Out of control for two model frames is a real interruption: no automatic change for this signal
    paused = DesireHelper(policy)
    frame(paused)
    frame(paused, active=False)
    frame(paused, active=False)
    frame(paused)
    for _ in range(40):
      self.assertEqual(frame(paused), (log.LaneChangeState.preLaneChange, "laneUnavailable"))
    self.assertEqual(frame(paused, state=car(leftBlinker=True, vEgo=left.vEgo, steeringPressed=True, steeringTorque=1))[0],
                     log.LaneChangeState.laneChangeStarting)

    # A signal that began while openpilot was not steering stays manual; a fresh signal is automatic again
    late = DesireHelper(policy)
    frame(late, active=False)
    frame(late, active=False)
    frame(late)
    for _ in range(40):
      self.assertEqual(frame(late), (log.LaneChangeState.preLaneChange, "laneUnavailable"))
    frame(late, state=car(vEgo=left.vEgo))
    self.assertIn(frames_until_automatic_start(late), (19, 20, 21))


if __name__ == "__main__":
  unittest.main()
