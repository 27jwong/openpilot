"""Vision-only lead braking from the lead's own speed track (Mazda): filter, limits, radard wiring."""
import math
import unittest
from types import SimpleNamespace
from unittest.mock import Mock, patch

import numpy as np

from openpilot.selfdrive.controls import radard
from openpilot.selfdrive.controls.radard import RADAR_TO_CAMERA, RadarD
from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib import long_mpc
from openpilot.selfdrive.controls.tests.test_adjacent_radar_transport import Captured, MONO, Sources
from openpilot.starpilot.longitudinal import vision_lead_accel as vla
from openpilot.starpilot.longitudinal.vision_lead_accel import (CONFIRM_ACCEL, EGO_DELAY_FRAMES, GAINS, MAX_MODEL_MULTIPLE,
                                                                 VisionLeadAccel)

DT = 0.05
HALF_SECOND = 10  # frames: a new car is the model alone this long


def drive(acc, frames, *, lead_accel=0.0, model_share=0.4, model_floor=None, v_lead0=25., ego_accel=None, d0=30.,
          noise=0.14, seed=0, lead=0, t0=0., ego_lag_frames=EGO_DELAY_FRAMES):
  """Feed one car for this many frames; returns (published, model a[0]) per frame.

  The lead brakes at lead_accel; the model reports model_share of it as a[0] (or model_floor if given) and its closing
  speed against the car's speed ego_lag_frames earlier, the way the CX-30's models line up with the wheels."""
  rng = np.random.default_rng(seed)
  ve = v_lead0
  hist = [ve] * (ego_lag_frames + 1)
  out, model = [], []
  d = d0
  for k in range(frames):
    t = t0 + k * DT
    v_lead = max(v_lead0 + lead_accel * k * DT, 0.)
    if ego_accel is not None and k:
      ve += ego_accel(t) * DT
    hist = hist[1:] + [ve]
    d += (v_lead - ve) * DT
    a0 = model_floor if model_floor is not None else model_share * lead_accel
    acc.observe_ego(ve)
    out.append(acc.update(lead, t, d, v_lead - hist[0] + rng.normal(0., noise), a0))
    model.append(a0)
  return np.array(out), np.array(model)


class TestFilter(unittest.TestCase):
  def test_gains_come_from_the_noise_numbers_and_are_stable(self):
    self.assertAlmostEqual(GAINS[0], 0.3005, places=3)
    self.assertAlmostEqual(GAINS[1], 1.0686, places=3)
    F = np.array([[1., DT], [0., 1.]])
    closed = (np.eye(2) - np.outer(GAINS, [1., 0.])) @ F
    self.assertLess(max(abs(np.linalg.eigvals(closed))), 1.)

  def test_the_cutoff_is_the_planners_model_path_cutoff(self):
    self.assertEqual(CONFIRM_ACCEL, -long_mpc.MODEL_LEAD_TRAJECTORY_MAX_LEAD_BRAKE)

  def test_a_steady_or_gently_slowing_lead_passes_the_model_through(self):
    for a0 in (0.3, 0., -0.3, -0.5):
      out, model = drive(VisionLeadAccel(), 600, model_floor=a0, seed=3)
      np.testing.assert_array_equal(out, model)

  def test_a_hard_braking_lead_is_seen_near_its_real_braking(self):
    out, model = drive(VisionLeadAccel(), 60, lead_accel=-3.5)
    self.assertAlmostEqual(model[-1], -1.4)
    # from a second on, within 0.7 m/s^2 of the real -3.5; never less braking than the model says
    self.assertLess(abs(np.median(out[20:]) + 3.5), 0.7)
    self.assertTrue(np.all(out <= model))
    # the first half second on a car is the model alone
    np.testing.assert_array_equal(out[:HALF_SECOND], model[:HALF_SECOND])

  def test_nothing_until_the_model_itself_says_the_lead_brakes(self):
    out, model = drive(VisionLeadAccel(), 60, lead_accel=-3.0, model_floor=-0.45)
    np.testing.assert_array_equal(out, model)
    out, _ = drive(VisionLeadAccel(), 60, lead_accel=-3.0, model_floor=-0.6)
    self.assertLess(out[-1], -2.0)

  def test_never_more_than_four_times_the_models_braking(self):
    out, model = drive(VisionLeadAccel(), 60, lead_accel=-8.0, model_floor=-0.6, v_lead0=30.)
    self.assertAlmostEqual(out[-1], MAX_MODEL_MULTIPLE * -0.6)
    self.assertTrue(np.all(out >= MAX_MODEL_MULTIPLE * model - 1e-9))

  def test_another_car_or_a_gap_starts_over(self):
    def braking_then(event):
      acc = VisionLeadAccel()
      out, _ = drive(acc, 40, lead_accel=-3.5)
      self.assertLess(out[-1], -2.5)
      after = event(acc)
      return np.array(after)
    v_end = 25. - 3.5 * 39 * DT
    t_end = 39 * DT
    cases = {
      'distance jump': lambda acc: [acc.update(0, t_end + (k + 1) * DT, 60., v_end - 25., -1.4) for k in range(HALF_SECOND)],
      'frames missing': lambda acc: [acc.update(0, t_end + 0.5 + k * DT, 25., v_end - 25., -1.4) for k in range(HALF_SECOND)],
      'speed step': lambda acc: [acc.update(0, t_end + (k + 1) * DT, 25., v_end - 25. - 3., -1.4) for k in range(HALF_SECOND)],
      'no lead': lambda acc: (acc.reset(0), [acc.update(0, t_end + (k + 1) * DT, 25., v_end - 25., -1.4)
                                             for k in range(HALF_SECOND)])[1],
    }
    for name, event in cases.items():
      with self.subTest(name):
        np.testing.assert_array_equal(braking_then(event), -1.4)

  def test_each_lead_keeps_its_own_filter(self):
    # a braking leadOne and a steady leadTwo, read in the same model frames
    acc = VisionLeadAccel()
    one, two = [], []
    for k in range(40):
      acc.observe_ego(25.)
      one.append(acc.update(0, k * DT, 30. - 3.5 * (k * DT) ** 2 / 2., -3.5 * k * DT, -1.4))
      two.append(acc.update(1, k * DT, 60., 0., -0.6))
    self.assertLess(one[-1], -2.5)
    np.testing.assert_array_equal(two, -0.6)

  def test_the_cars_own_braking_is_not_read_as_the_lead_braking(self):
    # the car brakes hard behind a steady lead; the model's closing speed lines up with the wheels 0.2 s back
    def ego_accel(t):
      return -3.0 * min(max((t - 1.0) / 0.5, 0.), 1.)  # into -3 m/s^2 over half a second from t = 1 s
    out, model = drive(VisionLeadAccel(), 60, model_floor=-0.6, ego_accel=ego_accel, noise=0.)
    np.testing.assert_allclose(out, model)
    # pairing it with the newest wheel speed instead would read the lead braking with the car
    with patch.object(vla, 'EGO_DELAY_FRAMES', 0):
      acc = VisionLeadAccel()
    out, _ = drive(acc, 60, model_floor=-0.6, ego_accel=ego_accel, noise=0.)
    self.assertLess(out.min(), -1.0)

  def test_bad_numbers_pass_the_model_through_and_start_over(self):
    acc = VisionLeadAccel()
    self.assertEqual(acc.update(0, 0., 30., 0., -1.0), -1.0)  # no wheel speed yet
    drive(acc, 40, lead_accel=-3.5)
    for bad in (float('nan'), float('inf')):
      self.assertEqual(acc.update(0, 2.05, bad, -7., -1.4), -1.4)
      self.assertEqual(acc.update(0, 2.10, 25., bad, -1.4), -1.4)
    self.assertTrue(math.isnan(acc.update(0, 2.15, 25., -7., float('nan'))))
    self.assertEqual(acc.update(0, 2.20, 25., -7., -1.4), -1.4)  # restarted: the model alone for now


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
      lead.v = [30.] * 6
      lead.a = [0.] * 6
      lead.xStd, lead.yStd, lead.vStd = [1.], [1.], [1.]
    return source

  def run_frames(self, radar, source, frames, lead_accel, model_share=0.4):
    d = 35.
    for k in range(frames):
      v_lead = 30. + lead_accel * k * DT
      d += (v_lead - 30.) * DT
      for lead in source.data['modelV2'].leadsV3:
        lead.x = [d + RADAR_TO_CAMERA] * 6
        lead.v = [v_lead] * 6
        lead.a = [model_share * lead_accel] * 6
      source.logMonoTime['modelV2'] = MONO + int(k * DT * 1e9)
      source.recv_frame['carState'] = k + 2
      radar.update(source, source['radarTracks'])

  def test_off_by_default_and_unchanged(self):
    radar = RadarD()
    self.assertIsNone(radar.vision_lead_accel)
    source = self.source()
    self.run_frames(radar, source, 40, -3.5)
    self.assertAlmostEqual(radar.radar_state.leadOne.aLeadK, -1.4, places=5)

  def test_both_leads_publish_the_measured_braking(self):
    radar = RadarD(vision_lead_accel=True)
    source = self.source()
    self.run_frames(radar, source, 40, -3.5)
    model = source['modelV2']
    for lead in (radar.radar_state.leadOne, radar.radar_state.leadTwo):
      self.assertLess(lead.aLeadK, -3.0)
      self.assertGreaterEqual(lead.aLeadK, MAX_MODEL_MULTIPLE * -1.4)
      self.assertAlmostEqual(lead.aLeadTau, 0.3, places=5)
      self.assertAlmostEqual(lead.vLead, 30. - 3.5 * 39 * DT, places=4)
      self.assertFalse(lead.radar)
    # the planner extrapolates this lead either way: the model already put it past the model-path cutoff
    self.assertIsNone(long_mpc.build_model_lead_trajectory(model.leadsV3[0], radar.radar_state.leadOne, 30., 30.))
    publisher = Captured()
    radar.publish(publisher)
    self.assertLess(publisher.messages['radarState'].radarState.leadOne.aLeadK, -3.0)

  def test_a_lead_the_model_calls_steady_keeps_the_model_path(self):
    radar = RadarD(vision_lead_accel=True)
    source = self.source()
    self.run_frames(radar, source, 40, -1.0, model_share=0.3)  # the model says -0.3: not braking enough to act on
    self.assertAlmostEqual(radar.radar_state.leadOne.aLeadK, -0.3, places=5)
    model = source['modelV2']
    self.assertIsNotNone(long_mpc.build_model_lead_trajectory(model.leadsV3[0], radar.radar_state.leadOne, 30., 30.))

  def test_main_measures_mazda_only(self):
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
        self.assertEqual(instances[0].vision_lead_accel is not None, expected)


if __name__ == '__main__':
  unittest.main()
