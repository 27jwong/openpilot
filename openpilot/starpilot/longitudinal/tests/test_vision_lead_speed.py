"""Online closing-speed calibration for vision-only leads (Mazda GEN2): learner, safety limits, radard wiring."""
import math
import unittest
from types import SimpleNamespace
from unittest.mock import Mock, patch

import numpy as np

from openpilot.selfdrive.controls import radard
from openpilot.selfdrive.controls.radard import RADAR_TO_CAMERA, RadarD
from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib import long_mpc
from openpilot.selfdrive.controls.tests.test_adjacent_radar_transport import Captured, MONO, Sources
from openpilot.starpilot.longitudinal.vision_lead_speed import CORRECTION_CAP, WINDOW_FRAMES, VisionLeadSpeedCalibration

DT = 0.05


def now(cal, v_ego, d_rel, v_rel, a_lead, lead=0):
  """The correction for one frame on its own (same timestamp twice is not a sequence, so no trend carries over)."""
  return cal.correction(lead, 0., v_ego, d_rel, v_rel, a_lead)


def held(cal, frames, v_ego, d_rel, v_rel, a_lead, lead=0, t0=10.):
  """The correction after this reading has been held for this many 20 Hz frames."""
  c = 0.
  for k in range(frames):
    c = cal.correction(lead, t0 + k * DT, v_ego, d_rel, v_rel, a_lead)
  return c


def follow(cal, frames, *, v_ego=30., gap=35., bias=-0.6, lead_speed=None, noise=0.3, seed=0, t0=0.):
  """Feed frames of one car. lead_speed(t) is the true lead speed; the model reports closing speed + bias."""
  rng = np.random.default_rng(seed)
  d = gap
  for k in range(frames):
    t = t0 + k * DT
    v_lead = v_ego if lead_speed is None else lead_speed(t)
    true_rel = v_lead - v_ego
    d += true_rel * DT
    cal.observe(t, .99, d + rng.normal(0, 0.8), true_rel + bias + rng.normal(0, noise), v_ego, 1.)
  return d


class TestLearner(unittest.TestCase):
  def test_learns_the_bias_of_a_steady_follow(self):
    cal = VisionLeadSpeedCalibration()
    follow(cal, 60 * WINDOW_FRAMES)
    self.assertEqual(cal.windows, 60)
    self.assertAlmostEqual(cal.bias(30., 35.), -0.6, delta=0.1)
    # applied while following steadily: the corrected closing speed is ~0, as the gap says
    self.assertAlmostEqual(-0.6 - now(cal, 30., 35., -0.6, 0.), 0., delta=0.1)

  def test_a_consistent_model_learns_nothing_while_the_lead_changes_speed(self):
    cal = VisionLeadSpeedCalibration()
    follow(cal, 60 * WINDOW_FRAMES, bias=0., lead_speed=lambda t: 30. + 2. * math.sin(t / 6.))
    self.assertGreater(cal.windows, 50)
    self.assertLess(abs(cal.bias(30., 35.)), 0.1)

  def test_a_new_car_or_a_dropped_frame_ends_the_window(self):
    cal = VisionLeadSpeedCalibration()
    follow(cal, WINDOW_FRAMES // 2, noise=0.)
    follow(cal, WINDOW_FRAMES // 2, gap=55., noise=0., t0=WINDOW_FRAMES // 2 * DT)  # jumped to a car 20 m farther
    self.assertEqual(cal.windows, 0)
    follow(cal, WINDOW_FRAMES // 2, noise=0., t0=100.)
    follow(cal, WINDOW_FRAMES // 2, noise=0., t0=100.5 + WINDOW_FRAMES // 2 * DT)  # 0.5 s of frames missing
    self.assertEqual(cal.windows, 0)
    follow(cal, WINDOW_FRAMES, noise=0., t0=200.)
    self.assertEqual(cal.windows, 1)

  def test_unusable_frames_are_not_learned_from(self):
    for kwargs in ({'v_ego': 2.}, {'gap': 120.}):
      cal = VisionLeadSpeedCalibration()
      follow(cal, 3 * WINDOW_FRAMES, **kwargs)
      self.assertEqual(cal.windows, 0, kwargs)
    cal = VisionLeadSpeedCalibration()
    for k in range(3 * WINDOW_FRAMES):
      cal.observe(k * DT, .4, 35., -.6, 30., 1.)  # low probability
      cal.observe(k * DT + 1e-3, .99, 35., -.6, 30., 6.)  # distance too uncertain
      cal.observe(k * DT + 2e-3, .99, float('nan'), -.6, 30., 1.)
    self.assertEqual(cal.windows, 0)


class TestCorrectionLimits(unittest.TestCase):
  def calibrated(self, bias=-0.6):
    cal = VisionLeadSpeedCalibration()
    cal.theta = np.array([bias, 0., 0.])
    return cal

  def test_none_at_crawl_and_stop_and_go_speeds(self):
    cal = self.calibrated()
    self.assertEqual(now(cal, 0., 8., 0., 0.), 0.)
    self.assertEqual(now(cal, 8., 15., 0., 0.), 0.)
    self.assertAlmostEqual(now(cal, 10., 20., 0., 0.), -0.3)
    self.assertAlmostEqual(now(cal, 12., 20., 0., 0.), -0.6)

  def test_urgent_leads_are_seen_exactly_as_the_model_reports_them(self):
    cal = self.calibrated()
    self.assertEqual(now(cal, 30., 20., -4., 0.), 0.)     # 5 s to impact
    self.assertEqual(now(cal, 30., 40., -.6, -1.2), 0.)   # lead braking at 1.2 m/s^2, at once
    self.assertAlmostEqual(now(cal, 30., 10., -1., 0.), -0.6 * (10. - 5.) / 7.)  # 10 s to impact: part way
    self.assertAlmostEqual(now(cal, 30., 40., 1., 0.), -0.6)  # pulling away: full

  def test_no_correction_while_really_closing_in(self):
    # the model under-reads a real approach, so the steady-following bias must not be removed there
    cal = self.calibrated()
    self.assertAlmostEqual(held(cal, 40, 30., 60., -1.1, 0.), -0.6)  # what the bias alone explains: full
    self.assertAlmostEqual(held(cal, 40, 30., 60., -1.6, 0.), -0.3)  # 1 m/s closing once corrected: half
    self.assertEqual(held(cal, 40, 30., 60., -2.1, 0.), 0.)          # 1.5 m/s closing once corrected: none

  def test_no_correction_while_the_lead_slows(self):
    cal = self.calibrated()
    self.assertAlmostEqual(held(cal, 40, 30., 40., -.6, -.1), -0.6)
    self.assertAlmostEqual(held(cal, 40, 30., 40., -.6, -.3), -0.3, delta=.01)
    self.assertEqual(held(cal, 40, 30., 40., -.6, -.5), 0.)

  def test_the_fades_follow_the_last_second_not_one_frame(self):
    cal = self.calibrated()
    held(cal, 40, 30., 40., -.6, 0.)
    # one frame that reads a hard approach and a slowing lead barely moves it
    self.assertLess(cal.correction(0, 10. + 40 * DT, 30., 40., -2.6, -.45), -0.5)
    # held for a second and a half, it is gone
    self.assertAlmostEqual(held(cal, 30, 30., 40., -2.6, -.45, t0=10. + 41 * DT), 0., delta=.02)
    # each lead keeps its own trend, and a new car or a gap in the frames starts over from its first reading
    self.assertAlmostEqual(cal.correction(1, 20., 30., 40., -.6, 0.), -0.6)
    self.assertAlmostEqual(cal.correction(0, 10. + 71 * DT, 30., 60., -.6, 0.), -0.6)  # 20 m farther: another car
    held(cal, 40, 30., 40., -2.6, 0., t0=30.)
    self.assertAlmostEqual(cal.correction(0, 30. + 39 * DT + .5, 30., 40., -.6, 0.), -0.6)  # 0.5 s of frames missing

  def test_capped_and_robust_to_bad_numbers(self):
    for sign in (-1., 1.):  # steady following, the model reading exactly its (capped) bias
      self.assertEqual(now(self.calibrated(5. * sign), 30., 35., sign * CORRECTION_CAP, 0.), sign * CORRECTION_CAP)
    cal = self.calibrated()
    for bad in (float('nan'), float('inf')):
      self.assertEqual(now(cal, bad, 35., 0., 0.), 0.)
      self.assertEqual(now(cal, 30., 35., bad, 0.), 0.)
      self.assertEqual(cal.correction(0, bad, 30., 35., 0., 0.), 0.)
    # a bad frame does not poison the trend
    held(cal, 20, 30., 35., -.6, 0.)
    cal.correction(0, 10. + 20 * DT, 30., 35., float('nan'), 0.)
    self.assertAlmostEqual(cal.correction(0, 10. + 21 * DT, 30., 35., -.6, 0.), -0.6)
    cal.theta = np.array([float('nan'), 0., 0.])
    self.assertEqual(now(cal, 30., 35., 0., 0.), 0.)


class TestRadardWiring(unittest.TestCase):
  def source(self, stamp=MONO):
    source = Sources(stamp)
    source.data['radarTracks'].init('errors')
    source.data['carState'].vEgo = 30.
    source.data['modelV2'].velocity.x = [30.] * 33
    for lead in source.data['modelV2'].leadsV3:
      lead.prob = .99
      lead.t = long_mpc.LEAD_T_IDXS_MODEL.tolist()
      lead.x = [35. + RADAR_TO_CAMERA] * 6
      lead.y = [0.] * 6
      lead.v = [29.4] * 6  # the model says closing at 0.6 m/s while its distance holds still
      lead.a = [0.] * 6
      lead.xStd, lead.yStd, lead.vStd = [1.], [1.], [1.]
    return source

  def test_off_by_default_and_unchanged(self):
    source = self.source()
    default = RadarD()
    default.update(source, source['radarTracks'])
    self.assertIsNone(default.vision_lead_speed)
    self.assertAlmostEqual(default.radar_state.leadOne.vRel, -0.6, places=5)

  def test_learns_from_the_primary_lead_and_corrects_both_consistently(self):
    radar = RadarD(vision_lead_speed_calibration=True)
    source = self.source()
    with patch.object(radard, 'cloudlog') as log:
      for k in range(WINDOW_FRAMES):
        source.logMonoTime['modelV2'] = MONO + int(k * DT * 1e9)
        source.recv_frame['carState'] = k + 2
        radar.update(source, source['radarTracks'])
    self.assertEqual(radar.vision_lead_speed.windows, 1)
    log.event.assert_called_once()
    self.assertEqual(log.event.call_args.args[0], 'vision_lead_speed')
    radar.vision_lead_speed.theta = np.array([-0.6, 0., 0.])
    radar.update(source, source['radarTracks'])
    model = source['modelV2']
    for lead, model_lead in zip((radar.radar_state.leadOne, radar.radar_state.leadTwo), model.leadsV3, strict=False):
      self.assertAlmostEqual(lead.vRel, 0., places=5)
      self.assertAlmostEqual(lead.vLead, 30., places=5)
      self.assertAlmostEqual(lead.vLeadK, 30., places=5)
      self.assertAlmostEqual(lead.dRel, 35., places=5)
      # the planner still recognises the corrected lead as this model frame's lead
      self.assertTrue(long_mpc.lead_horizon_matches(model_lead, lead, 30., 30.))
    self.assertIsNotNone(long_mpc.build_model_lead_trajectory(model.leadsV3[0], radar.radar_state.leadOne, 30., 30.))
    publisher = Captured()
    radar.publish(publisher)
    self.assertAlmostEqual(publisher.messages['radarState'].radarState.leadOne.vLead, 30., places=5)

  def test_main_calibrates_mazda_only(self):
    for brand, expected in (('mazda', True), ('toyota', False)):
      with self.subTest(brand=brand):
        cp = SimpleNamespace(brand=brand, carFingerprint='test', radarDelay=0., radarUnavailable=brand == 'mazda',
                             openpilotLongitudinalControl=True, notCar=False, dashcamOnly=False, passive=False)
        source = self.source()
        source.update = Mock(side_effect=(None, StopIteration))
        instances = []

        def create(*args, instances=instances, **kwargs):
          instances.append(RadarD(*args, **kwargs))
          return instances[-1]

        params = SimpleNamespace(get=lambda *args, **kwargs: b'cp', get_bool=lambda *args, **kwargs: False)
        with patch.object(radard, 'Params', return_value=params), \
             patch.object(radard, 'config_realtime_process'), patch.object(radard, 'set_core_affinity'), \
             patch.object(radard, 'feature_enabled', return_value=False), \
             patch.object(radard, 'voacc_selected', return_value=False), \
             patch.object(radard.messaging, 'log_from_bytes', return_value=cp), \
             patch.object(radard.messaging, 'SubMaster', return_value=source), \
             patch.object(radard.messaging, 'PubMaster', return_value=Captured()), \
             patch.object(radard, 'RadarD', side_effect=create):
          with self.assertRaises(StopIteration):
            radard.main()
        self.assertEqual(instances[0].vision_lead_speed is not None, expected)


if __name__ == '__main__':
  unittest.main()
