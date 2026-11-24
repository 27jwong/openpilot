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
    self.assertAlmostEqual(-0.6 - cal.correction(30., 35., -0.6, 0.), 0., delta=0.1)

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
    self.assertEqual(cal.correction(0., 8., 0., 0.), 0.)
    self.assertEqual(cal.correction(8., 15., 0., 0.), 0.)
    self.assertAlmostEqual(cal.correction(10., 20., 0., 0.), -0.3)
    self.assertAlmostEqual(cal.correction(12., 20., 0., 0.), -0.6)

  def test_urgent_leads_are_seen_exactly_as_the_model_reports_them(self):
    cal = self.calibrated()
    self.assertEqual(cal.correction(30., 20., -4., 0.), 0.)     # 5 s to impact
    self.assertEqual(cal.correction(30., 40., -1., -1.2), 0.)   # lead braking at 1.2 m/s^2
    self.assertAlmostEqual(cal.correction(30., 40., -4., 0.), -0.6 * (10. - 5.) / 7.)  # 10 s: part way
    self.assertAlmostEqual(cal.correction(30., 40., 1., 0.), -0.6)  # pulling away: full

  def test_capped_and_robust_to_bad_numbers(self):
    for bias in (-5., 5.):
      self.assertEqual(abs(self.calibrated(bias).correction(30., 35., 0., 0.)), CORRECTION_CAP)
    cal = self.calibrated()
    for bad in (float('nan'), float('inf')):
      self.assertEqual(cal.correction(bad, 35., 0., 0.), 0.)
      self.assertEqual(cal.correction(30., 35., bad, 0.), 0.)
    cal.theta = np.array([float('nan'), 0., 0.])
    self.assertEqual(cal.correction(30., 35., 0., 0.), 0.)


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
