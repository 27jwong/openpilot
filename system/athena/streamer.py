"""Device side of Konik's live view.

Konik's browser client builds the WebRTC offer and calls athena's startStream; we answer it.
Video is the already-H264-encoded stream from stream_encoderd, handed to aiortc as packets so
nothing is transcoded on the device.

This deliberately does not go through system/webrtc/webrtcd.py: that needs teleoprtc, which
isn't vendored in this tree.
"""
import asyncio
import fractions
import logging
import threading
import time

import av
import aiortc
from aiortc import RTCConfiguration, RTCIceServer, RTCPeerConnection, RTCRtpSender, RTCSessionDescription
from aiortc.mediastreams import VIDEO_CLOCK_RATE, VIDEO_TIME_BASE

import cereal.messaging as messaging
from openpilot.common.params import Params


logger = logging.getLogger("athena.streamer")

CAMERA_TO_SOCK = {
  "driver": "livestreamDriverEncodeData",
  "wideRoad": "livestreamWideRoadEncodeData",
  "road": "livestreamRoadEncodeData",
}
# Konik's client only ever offers one recvonly video track and labels it "wideRoad"
DEFAULT_CAMERA = "wideRoad"
ICE_SERVERS = [RTCIceServer(urls="stun:stun.l.google.com:19302")]
ICE_GATHER_TIMEOUT_S = 8
FRAME_WAIT_S = 0.005


def _h264_capabilities():
  """aiortc registers H264 under several profiles; keep them all so the browser can match one."""
  codecs = RTCRtpSender.getCapabilities("video").codecs
  return [c for c in codecs if c.mimeType.lower() == "video/h264"]


class LiveStreamTrack(aiortc.MediaStreamTrack):
  """Feeds stream_encoderd's H264 packets straight into aiortc, no re-encode."""
  kind = "video"

  def __init__(self, camera_type: str):
    assert camera_type in CAMERA_TO_SOCK
    super().__init__()
    self._sock = messaging.sub_sock(CAMERA_TO_SOCK[camera_type], conflate=True)
    self._time_base: fractions.Fraction = VIDEO_TIME_BASE
    self._clock_rate: int = VIDEO_CLOCK_RATE
    self._t0_ns = time.monotonic_ns()

  async def recv(self):
    while True:
      msg = messaging.recv_one_or_none(self._sock)
      if msg is not None:
        break
      await asyncio.sleep(FRAME_WAIT_S)

    evta = getattr(msg, msg.which())
    packet = av.Packet(evta.header + evta.data)
    packet.time_base = self._time_base
    packet.pts = ((time.monotonic_ns() - self._t0_ns) * self._clock_rate) // 1_000_000_000
    return packet


class StreamManager:
  """Owns the peer connections on a private asyncio loop, driven from athena's worker threads."""

  def __init__(self):
    self._loop = asyncio.new_event_loop()
    self._pc: RTCPeerConnection | None = None
    self._params = Params()
    self._thread = threading.Thread(target=self._run_loop, name="athena_streamer", daemon=True)
    self._thread.start()

  def _run_loop(self) -> None:
    asyncio.set_event_loop(self._loop)
    self._loop.run_forever()

  def answer(self, offer_sdp: str, timeout: float) -> str:
    """Blocking: hand an offer to the loop and get the answer SDP back."""
    future = asyncio.run_coroutine_threadsafe(self._answer(offer_sdp), self._loop)
    return future.result(timeout=timeout)

  def stop(self) -> None:
    if self._loop.is_closed():
      return
    asyncio.run_coroutine_threadsafe(self._close(), self._loop).result(timeout=5)

  async def _close(self) -> None:
    if self._pc is not None:
      pc, self._pc = self._pc, None
      try:
        await pc.close()
      except Exception:
        logger.exception("failed closing peer connection")

  async def _answer(self, offer_sdp: str) -> str:
    # one viewer at a time; a new offer replaces the old session
    await self._close()

    pc = RTCPeerConnection(RTCConfiguration(iceServers=ICE_SERVERS))
    self._pc = pc

    @pc.on("connectionstatechange")
    async def on_connectionstatechange():
      logger.info("live view connection state %s", pc.connectionState)
      # "disconnected" is transient and often recovers, so only tear down on a terminal state
      if pc.connectionState in ("failed", "closed"):
        if self._pc is pc:
          self._pc = None
          self._params.put_bool("LiveView", False)
        await pc.close()

    try:
      await pc.setRemoteDescription(RTCSessionDescription(sdp=offer_sdp, type="offer"))

      # the client offers recvonly video; fill it with a camera and keep it on H264 so
      # stream_encoderd's packets pass straight through instead of being transcoded
      h264 = _h264_capabilities()
      for transceiver in pc.getTransceivers():
        if transceiver.kind != "video":
          continue
        transceiver.direction = "sendonly"
        if h264:
          transceiver.setCodecPreferences(h264)
      pc.addTrack(LiveStreamTrack(DEFAULT_CAMERA))

      await pc.setLocalDescription(await pc.createAnswer())

      # no trickle ICE in this protocol, so the answer has to carry every candidate
      deadline = time.monotonic() + ICE_GATHER_TIMEOUT_S
      while pc.iceGatheringState != "complete" and time.monotonic() < deadline:
        await asyncio.sleep(0.1)

      return pc.localDescription.sdp
    except Exception:
      await self._close()
      raise
