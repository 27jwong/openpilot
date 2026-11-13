"""Corroborated lead departure clears only an existing CEM model hold."""
import math

from opendbc.car import structs
from openpilot.selfdrive.controls.radard import get_RadarState_from_vision
from openpilot.starpilot.model_geometry import STANDSTILL_RELEASE_SPEED_MPS
from openpilot.selfdrive.modeld.constants import ModelConstants
from openpilot.starpilot.conditional_mode.projection import _standstill_path_extent
from openpilot.starpilot.conditional_mode.stop import STOP_MODEL_RELEASE_MARGIN_M

SOURCES = ('carState', 'carControl', 'controlsState', 'selfdriveState', 'radarState', 'modelV2')
MAX_AGE_NS = 150_000_000
CLEAR_DWELL_NS = 500_000_000
LEAD_PASS_MARGIN_M = 20.0  # Plan must pass the departing lead's current spot by this much.
# Nearest a departing lead may be. Upstream's 6.3 m is openpilot's own 6 m stop distance plus a
# margin, so it reads "the lead has left its stopped spot". A CX-30 stop under blended ACC rests
# wherever MRCC put it: 4-5.6 m behind the lead on 2026-11-11, 1.8-3.6 m on 2026-11-13. Any floor
# above the resting gap makes the lead cover the difference before the dwell can even start
# (0.4-1.6 s on 11-13, the driver pressed the gas first every time). The lead's speed over the
# dwell, the model path and the planner's own go decision already prove the departure, so the
# floor only rejects a lead with no usable distance.
LEAD_MIN_GAP_M = 1.0
# Lowest speed the model may report anywhere on its plan. At a standstill its near-term speed
# (peaking ~0.16 s out) jitters down to -0.10 m/s on the CX-30; upstream's -0.05 failed 3-6 % of
# standstill frames and one such frame restarts the dwell (three times in 0.8 s on 2026-11-13).
# The plan from 2 s on must still be >= 0.3 m/s and rising, which is what rules out a reversing plan.
MODEL_SPEED_FLOOR_MPS = -0.2


def _supported(cp):
  try:
    return bool(cp.openpilotLongitudinalControl and not (cp.passive or cp.dashcamOnly or cp.notCar))
  except (AttributeError, TypeError, ValueError):
    return False


def owner_for(cp):
  return StoppedLeadRelease() if _supported(cp) else None


def _min_release_extent_m(commit_distance_m, leads):
  """Shortest model path that still proves the committed stop is behind us.

  `leads` are already corroborated departing leads (1-18 m, vLead >= 0.6, not
  braking, model-correlated). A model that follows one of them plans ~40-48 m
  over its 10 s horizon, so the raw 50 m stop distance alone can never clear.
  Instead the plan must carry ego past the farthest lead's current position.
  """
  return max(commit_distance_m, max(lead.dRel for lead in leads) + LEAD_PASS_MARGIN_M) + STOP_MODEL_RELEASE_MARGIN_M


class StoppedLeadRelease:
  def __init__(self):
    self.reset()

  def reset(self):
    self.key = None
    self.since_ns = self.last_ns = self.model_ns = 0

  def sample(self, sm, cp, planner, detector, *, now_ns, drive_id):
    try:
      car, model, control = sm['carState'], sm['modelV2'], sm['carControl']
      if (not _supported(cp) or type(now_ns) is not int or type(drive_id) is not int or not 0 < drive_id < now_ns or
          not detector.committed or not detector.standstill_committed or detector.standstill_reason == 'sign' or
          not all(sm.valid[name] and sm.alive[name] and drive_id < sm.logMonoTime[name] <= now_ns and
                  now_ns - sm.logMonoTime[name] <= MAX_AGE_NS and
                  drive_id < int(sm.recv_time[name] * 1e9) <= now_ns and
                  now_ns - int(sm.recv_time[name] * 1e9) <= MAX_AGE_NS for name in SOURCES) or
          not car.canValid or car.canTimeout or car.vehicleSensorsInvalid or car.accFaulted or
          car.doorOpen or car.seatbeltUnlatched or car.regenBraking or
          not car.standstill or car.vEgoRaw != 0. or car.gearShifter != structs.CarState.GearShifter.drive or
          not math.isfinite(car.vEgo) or abs(car.vEgo) > STANDSTILL_RELEASE_SPEED_MPS or
          car.gasPressed or car.brakePressed or not control.enabled or not control.longActive or
          not sm['selfdriveState'].enabled or sm['controlsState'].forceDecel or
          planner.mpc.solution_status != 0 or type(planner.output_should_stop) is not bool or
          not math.isfinite(planner.output_a_target) or
          planner.force_stop_plan.forcing or planner.force_stop_plan.should_stop or
          model.action.shouldStop or not math.isfinite(model.action.desiredAcceleration) or model.action.desiredAcceleration < .3):
        self.reset()
        return False
      velocities, times = tuple(model.velocity.x), tuple(ModelConstants.T_IDXS)
      if len(velocities) != 33 or len(times) != 33 or not all(math.isfinite(v) and MODEL_SPEED_FLOOR_MPS <= v <= 100. for v in velocities):
        self.reset()
        return False
      tail = [v for v, t in zip(velocities, times, strict=True) if t >= 2.]
      if not tail or min(tail) < .3 or tail[-1] < 5. or any(b < a for a, b in zip(tail, tail[1:], strict=False)):
        self.reset()
        return False
      leads = [(slot, lead) for slot, lead in enumerate((sm['radarState'].leadOne, sm['radarState'].leadTwo)) if lead.present]
      if not leads or any(lead.radar and lead.radarTrackId < 0 or not all(math.isfinite(v) for v in
          (lead.dRel, lead.vLead, lead.aLeadK, lead.yRel, lead.modelProb)) or
          not LEAD_MIN_GAP_M <= lead.dRel <= 18. or lead.vLead < .6 or lead.aLeadK < -.15 or
          abs(lead.yRel) > 1. or lead.modelProb < .95 for _, lead in leads):
        self.reset()
        return False
      for slot, lead in leads:
        if not lead.radar:
          if lead.radarTrackId != -1:
            self.reset()
            return False
          source = model.leadsV3[slot]
          projected = get_RadarState_from_vision(source, car.vEgo, model.velocity.x[0], source.prob)
          if (source.prob < .95 or not math.isfinite(source.prob) or
              any(not math.isfinite(projected[field]) or abs(projected[field] - getattr(lead, field)) > .5
                  for field in ('dRel', 'yRel', 'vLead', 'aLeadK'))):
            self.reset()
            return False
      extent = _standstill_path_extent(model)
      if extent is None or extent < _min_release_extent_m(detector.commit_distance_m, [lead for _, lead in leads]):
        self.reset()
        return False
      key = (drive_id, cp.carFingerprint, tuple((str(config.safetyModel), config.safetyParam) for config in cp.safetyConfigs),
             tuple((slot, bool(lead.radar), lead.radarTrackId if lead.radar else -1) for slot, lead in leads))
      stamp = int(sm.logMonoTime['modelV2'])
      if (key != self.key or not self.last_ns < now_ns <= self.last_ns + MAX_AGE_NS or stamp <= self.model_ns):
        self.reset()
        # Unknown derived geometry cannot establish a new light-hold proof.
        if detector.standstill_reason != 'light':
          return False
        self.key, self.since_ns = key, now_ns
      self.last_ns, self.model_ns = now_ns, stamp
      # Confirm the scene while MPC settles; its current braking/stop decision
      # remains an unconditional release veto. No target acceleration is raised.
      return bool(now_ns - self.since_ns >= CLEAR_DWELL_NS and
                  detector.standstill_reason == 'light' and not planner.output_should_stop and planner.output_a_target > .1)
    except (AttributeError, KeyError, IndexError, TypeError, ValueError, OverflowError, RuntimeError):
      self.reset()
      return False
