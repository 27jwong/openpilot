"""Actual daemon publication at recorded cd7 clocks; no road-effect claim."""
from dataclasses import replace
from types import SimpleNamespace

import pytest

from openpilot.starpilot.flm import steerlearnerd
from openpilot.starpilot.flm.live_trial_wire import read_tracking_source
from openpilot.starpilot.flm.tests.test_live_tracking_daemon import LoopComplete, NoLegacyFitter, RecordingActor, source

OLD_READ = 1815302996615
OLD_SEND = 1815522269392
FRESH_READ = 1815523166765
CONTROL = 1815812717640


def publication(monkeypatch, *, changes=None, read_cost=0, retained=None, original_source=None):
  now = [OLD_SEND]
  old = source(OLD_READ) if original_source is None else original_source
  sent, reads, interruptions = [], [], []
  actor, fitter = RecordingActor(), NoLegacyFitter()
  actor.interrupt = lambda reason, **kwargs: interruptions.append(reason)
  fitter.cancel = lambda reason: interruptions.append(reason)
  if retained is not None:
    expired = retained.endswith('expired')
    pending = SimpleNamespace(observed_ns=OLD_SEND-200_000_000 if expired else OLD_SEND,
      expires_ns=OLD_SEND-1 if expired else OLD_SEND+100_000_000)
    if retained.startswith('trial'):
      monkeypatch.setattr(actor, 'tick', lambda *args, **kwargs: pending)
    else:
      original = steerlearnerd._LiveHistoryRestore
      class History(original):
        def __init__(self, priors):
          super().__init__(priors)
          self.offer = pending
        def advance(self, *args, **kwargs):
          pass
      monkeypatch.setattr(steerlearnerd, '_LiveHistoryRestore', History)

  def refresh(stamp):
    reads.append(stamp)
    fresh = replace(old, observed_ns=FRESH_READ, expires_ns=FRESH_READ+500_000_000, **(changes or {}))
    now[0] = FRESH_READ + read_cost
    return fresh

  def send(topic, message):
    sent.append((topic, message.as_reader()))
    if topic == 'autoSteeringLearning':
      raise LoopComplete

  sm = SimpleNamespace(updated={'deviceMotion': False}, logMonoTime={'deviceMotion': 0}, update=lambda *args: None)
  monkeypatch.setattr(steerlearnerd, 'drain', lambda *args: None)
  with pytest.raises(LoopComplete):
    steerlearnerd.run(SimpleNamespace(), sm, object(), object(), SimpleNamespace(send=send),
      None, None, lambda stamp: old, None, None, None, None, fitter, None,
      clock=lambda: now[0], live_actor=actor, publication_source_reader=refresh)
  assert actor.closed and fitter.closed
  assert reads == [OLD_SEND]  # One existing read; no new settings/Params work.
  tracking = [message for topic, message in sent if topic == 'starpilotSteeringTracking']
  return old, tracking, sent[-1][1], interruptions


def test_actual_daemon_forwards_real_refreshed_source_to_tracking_at_recorded_clocks(monkeypatch):
  old, tracking, learning, _ = publication(monkeypatch)
  assert len(tracking) == 2
  assert read_tracking_source(tracking[0].starpilotSteeringTracking, now_ns=CONTROL, cp_sha256=old.cp_sha256) is None
  current = read_tracking_source(tracking[1].starpilotSteeringTracking, now_ns=CONTROL, cp_sha256=old.cp_sha256)
  assert current == replace(old, observed_ns=FRESH_READ, expires_ns=FRESH_READ+500_000_000)
  assert tracking[1].starpilotSteeringTracking.sourceReceipt.to_dict() == learning.autoSteeringLearning.learning.sourceReceipt.to_dict()
  assert all(message.starpilotSteeringTracking.trial.version == message.starpilotSteeringTracking.restore.version == 0 for message in tracking)
  assert read_tracking_source(tracking[1].starpilotSteeringTracking,
    now_ns=FRESH_READ+500_000_001, cp_sha256=old.cp_sha256) is None


def test_read_that_outlives_its_real_lease_does_not_refresh_tracking(monkeypatch):
  _, tracking, learning, _ = publication(monkeypatch, read_cost=500_000_001)
  assert len(tracking) == 1
  assert learning.autoSteeringLearning.learning.status == 'unavailable'


@pytest.mark.parametrize('changes', [
  {'enabled': False, 'source_epoch': 'f'*64},
  {'manual': True, 'source_epoch': 'f'*64},
  {'reset': 'new-reset', 'source_epoch': 'f'*64},
  {'calibration_generation': 'f'*32, 'source_epoch': 'f'*64},
  {'cp_sha256': 'f'*64},
])
def test_refreshed_configuration_rejection_cannot_publish_old_context(monkeypatch, changes):
  _, tracking, learning, interruptions = publication(monkeypatch, changes=changes)
  assert len(tracking) == 1
  assert learning.autoSteeringLearning.learning.status == 'unavailable'
  assert interruptions


@pytest.mark.parametrize('retained', ['trial-live', 'trial-expired', 'restore-live', 'restore-expired'])
def test_retained_trial_or_restore_blocks_extra_neutral_publication(monkeypatch, retained):
  _, tracking, _, _ = publication(monkeypatch, retained=retained)
  assert all(message.starpilotSteeringTracking.sourceReceipt.observedMonoTime == OLD_READ for message in tracking)


def test_actual_controls_capture_recovers_from_forwarded_receipt_without_applying_a_correction(monkeypatch):
  from openpilot.cereal import messaging
  from openpilot.starpilot.flm.tests import test_live_trial_global_runtime as consumer
  from openpilot.starpilot.flm.tests.test_collection_metadata_frequency import capture

  # Actual Controls/GlobalRuntime/LaC and ordinary native permission fixture.
  # Only the source-receipt clocks are the recorded cd7 values; this is not a
  # physical response or learned-application qualification.
  monkeypatch.setattr(consumer, 'BASE', CONTROL-60*consumer.DT)
  with consumer.live_controls() as fixture:
    _, tracking, _, _ = publication(monkeypatch, original_source=fixture.source(OLD_READ))
    assert len(tracking) == 2

    def receive(message, stamp):
      event = message.as_builder()
      event.logMonoTime = OLD_SEND if message is tracking[0] else FRESH_READ
      fixture.controls.sm.update_msgs(stamp/1e9, [messaging.log_from_bytes(event.to_bytes())])

    receive(tracking[0], CONTROL)
    now, _ = fixture.frame(publish=False)
    assert now == CONTROL
    assert not capture(fixture, now)
    fixture.assert_neutral()

    receive(tracking[1], CONTROL+consumer.DT)
    now, _ = fixture.frame(publish=False)
    assert capture(fixture, now)
    assert fixture.runtime.source.observed_ns == FRESH_READ
    fixture.assert_neutral()

    while now <= FRESH_READ+500_000_000:
      now, _ = fixture.frame(publish=False)
    assert not capture(fixture, now)
    fixture.assert_neutral()
