"""Numerical continuity checks only, not actual native/command authority."""

from dataclasses import replace
import unittest
from openpilot.starpilot.flm.tests.test_tracking_source_context import owner, refresh, source_ack
from openpilot.starpilot.flm.tests.test_live_tracking import episode


def active_study():
  o = owner()
  for i in range(5):
    e = replace(episode(o, i, 'ff_gain_right'), normalization_accel=0.5)
    refresh(o, e.end_ns)
    o.observe_episode(e, now_ns=e.end_ns)
  assert o.offer_state is not None
  now = e.end_ns
  offer = o.offer(now)
  assert source_ack(o, offer, now + 10_000_000)
  o.future = [replace(episode(o, 20 + i, 'ff_gain_right', profile=offer.target_sha256), normalization_accel=0.5) for i in range(2)]
  return o, o.future[-1].end_ns


class SourceStudyGaps(unittest.TestCase):
  def test_each_transient_keeps_completed_scores_and_original_deadline_only(self):
    for reason in ('current_evidence_delayed', 'learning_evidence_delayed', 'fresh_physical_residual_window_required',
                   'consumed_source_mismatch'):
      with self.subTest(reason=reason):
        o, now = active_study()
        before = tuple(o.before)
        future = tuple(o.future)
        original = o.offer_state.copy()
        o.interrupt(reason)
        assert o.offer_state['phase'] == 'rollback' and not o.before and not o.future
        assert o.paused_intent[1:] == (before, future)
        refresh(o, now + 200_000_000)
        rollback = o.offer(now + 200_000_000)
        assert source_ack(o, rollback, now + 210_000_000)
        assert o.offer_state['phase'] == 'trial' and o.offer_state['trial_id'] != original['trial_id']
        assert o.offer_state['deadline_ns'] == original['deadline_ns']
        assert tuple(o.before) == before and tuple(o.future) == future and o.settled_ns is None
        assert not any(o.accepted.values())

  def test_normalized_join_clock_gap_keeps_inert_study_but_wrong_domain_does_not(self):
    from openpilot.starpilot.flm.fullrate_collector import FullrateCollector
    from openpilot.starpilot.flm.tests.test_fullrate_pipeline import BASE, SyntheticAdapter, snapshots
    from types import SimpleNamespace

    for wrong_domain in (False, True):
      with self.subTest(wrong_domain=wrong_domain):
        o, now = active_study()
        original = o.offer_state.copy()
        scores = tuple(o.before), tuple(o.future)
        c = FullrateCollector(SyntheticAdapter())
        c.adapter.command_domain = 'normalized_torque'
        c.live_tracking = SimpleNamespace(interrupt=o.interrupt)
        control, output, _pose = snapshots(BASE)
        control = replace(control, domain='normalized_torque')
        output = replace(output, domain='wrong' if wrong_domain else 'normalized_torque',
                         state_ns=control.state_ns, apply_ns=control.control_ns+30_000_001)
        c._join_emission(control, output)
        assert c.reason == 'consumed_source_mismatch' and not c.ready
        assert o.offer_state['phase'] == 'rollback' and o.settled_ns is None
        assert not o.before and not o.future and not any(o.accepted.values())
        if wrong_domain:
          assert o.paused_intent is None
        else:
          assert o.paused_intent[1:] == scores
        refresh(o, now+200_000_000)
        rollback = o.offer(now+200_000_000)
        assert source_ack(o, rollback, now+210_000_000)
        if wrong_domain:
          assert o.offer_state['phase'] == 'rollback' and o.offer_state['accepted']
        else:
          assert o.offer_state['phase'] == 'trial' and o.offer_state['trial_id'] != original['trial_id']
          for field in ('started_ns', 'deadline_ns', 'alpha', 'required_episodes', 'target', 'baseline'):
            assert o.offer_state[field] == original[field]
          assert (tuple(o.before), tuple(o.future)) == scores and o.settled_ns is None

  def test_context_change_drops_study_instead_of_resuming(self):
    o, now = active_study()
    o.interrupt('source_configuration_changed', context_changed=True)
    assert o.paused_intent is None and not o.before and not o.future
    refresh(o, now + 200_000_000)
    rollback = o.offer(now + 200_000_000)
    assert source_ack(o, rollback, now + 210_000_000)
    assert o.offer_state['phase'] == 'rollback' and o.offer_state['accepted']
    assert not any(o.accepted.values())


class SourceComparisonProgress(unittest.TestCase):
  def test_ended_maneuver_releases_comparison_and_allows_new_discovery(self):
    o = owner()
    # Two left windows select an experiment but no target is applied yet.
    for i in range(2):
      e = replace(episode(o, i, 'ff_gain_left'), normalization_accel=0.5)
      refresh(o, e.end_ns)
      o.observe_episode(e, now_ns=e.end_ns)
    assert o.discovery is not None and o.offer_state is None
    old = o.discovery.copy()
    # New right-turn geometry must not become the old left score baseline.
    for i in range(3, 11):
      e = replace(episode(o, i, 'ff_gain_right'), normalization_accel=0.5)
      refresh(o, e.end_ns)
      o.observe_episode(e, now_ns=e.end_ns)
      if o.offer_state is not None:
        break
    assert o.offer_state is not None and o.offer_state['names'] == ('ff_gain_right',)
    assert len(o.before) == 3 and all(e.direction == -1 for e in o.before)
    assert all(e.start_ns > old['selected_end_ns'] for e in o.before)
    assert not any(o.accepted.values())


class SourceRoadRegime(unittest.TestCase):
  def test_waveform_and_amplitude_change_still_match_same_source_regime(self):
    from openpilot.starpilot.flm.live_tracking import _matches

    o = owner()
    a = replace(episode(o, 1, 'center_taper_max', kind='subtle'), normalization_accel=0.1)
    b = replace(a, goal_rms=0.02, goal_shape=(0.9, -0.8, 0.5, -0.2, 0.1, 0.0, -0.6, 0.4))
    assert _matches(a, b)
    assert not _matches(a, replace(b, speed_low=5.0, speed_high=6.0))
    assert not _matches(a, replace(b, kind='large'))
    large = replace(episode(o, 2, 'ff_gain_left'), normalization_accel=0.5)
    assert not _matches(large, replace(large, direction=-1))
    assert not _matches(replace(a, normalization_accel=None), replace(b, normalization_accel=None))


class SourceActiveRegime(unittest.TestCase):
  def test_ended_active_regime_rolls_back_without_calling_it_improvement(self):
    o, now = active_study()
    state = o.offer_state.copy()
    e = replace(episode(o, 40, 'center_taper_max', kind='subtle', profile=o._hash(state['target'])), normalization_accel=0.1)
    refresh(o, e.end_ns)
    assert not o.observe_episode(e, now_ns=e.end_ns)
    assert o.offer_state['phase'] == 'rollback' and o.reason == 'trial_input_regime_ended'
    assert not any(o.accepted.values()) and o.offer_state['target'] == state['baseline']
    assert o.selector_hint is None and o.paused_intent is None


class SourceRampFreshness(unittest.TestCase):
  def test_known_ramp_preserves_original_pair_without_admitting_unknown_map(self):
    from types import SimpleNamespace
    from openpilot.starpilot.flm.tests.test_tracking_pose_interval import subject, pair

    o = subject()
    rows = [pair(o, 1_000_000_000), pair(o, 1_010_000_000)]
    rows[1][0].live_applied_coordinates = (0.00005,) + (0.0,) * 29
    pose = SimpleNamespace(source_lower_ns=1_005_000_000, source_upper_ns=1_015_000_000)
    chosen = o.original_pose_pair(rows, pose)
    assert chosen[0] is rows[0][0] and chosen[1].apply_ns == 1_000_000_000
    rows[1][0].live_applied_coordinates = ()
    rows[1][0].applied_coordinates = (0.01, 0.0)
    assert o.original_pose_pair(rows, pose) is None

  def test_plant_mode_still_requires_one_unambiguous_emission(self):
    from types import SimpleNamespace
    from openpilot.starpilot.flm.tests.test_tracking_pose_interval import subject, pair

    o = subject(False)
    rows = [pair(o, 1_000_000_000), pair(o, 1_010_000_000)]
    pose = SimpleNamespace(source_lower_ns=1_005_000_000, source_upper_ns=1_015_000_000)
    assert o.original_pose_pair(rows, pose) is None
