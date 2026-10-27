"""Jetlink's warp (jetlink.openpilot.warp.Warp's interface) over the small model's own camera warp.

The stock driving warp Jetlink ships with is a legacy9d pickle, which a native02e modeld cannot load. A catalog
model on the policy image pipeline already runs the same warp every frame, compiled for whichever runtime it was
selected under, and makes the frame the link sends: both cameras as (2, 6, model_h/2, model_w/2) uint8, the same
YUV layout as examples/openpilot/compile_warp.py's yuv420 warp. Reusing it keeps the link on the user's small
model's runtime and loads no second warp.

Only one of the small and the large model runs a frame (JoiningModelState), so the warp JIT, its inputs and its
output buffer are never in use by both at once. The small model rewrites its transforms every frame it runs.

tinygrad maps a QCOM output write-combined, which the CPU reads uncached: 2.9 ms to copy the frame out. As
Jetlink's own warp does (jetlink.openpilot.warp.Warp), the output moves to IO-coherent write-back memory
(warp.make_coherent) and the link sends straight from it, 0.22 ms. The warp has run by now, so it links again on
its next call. TinyJit's own per-call input checks are left alone: 0.05 ms a call on a Mac under this tinygrad.
"""
from __future__ import annotations

import numpy as np

PROBE = 77


class CatalogWarp:
  def __init__(self, small, geometry: tuple[int, int]):
    descriptor = small.camera_warp_descriptor
    if descriptor is None:
      raise RuntimeError("the small model's image history is not on the policy pipeline: it has no shared warp")
    model_w, model_h = geometry
    shape = (2, 6, model_h // 2, model_w // 2)
    if tuple(descriptor.output_shape) != shape or descriptor.dtype.itemsize != 1:
      raise RuntimeError(f"the small model's warp makes {descriptor.output_shape} {descriptor.dtype}, the link sends {shape} uint8")
    self._small = small
    # before modeld goes realtime: a blank frame finds the output buffer every frame writes (the small model's
    # warmup captured the JIT, which returns the same one every call), then a flat frame proves `output` reads it
    # live. as_memoryview() hands back a one-off copy where it finds no mapping, which would send the blank frame
    # forever; a flat frame under the identity samples to itself
    frames = [np.zeros(small.frame_buf_size, dtype=np.uint8) for _ in small.vision_input_names]
    pointers = tuple(frame.ctypes.data for frame in frames)
    eye = np.eye(3, dtype=np.float32)
    buffer = small.warp_frames(pointers, eye, eye).uop.base.buffer
    self._device = buffer.allocator.dev
    # why each frame is copied out, None when it is not
    self.copied = None if buffer.device.startswith("QCOM") else f"a {buffer.device} buffer"
    if self.copied is None:
      from jetlink.openpilot.warp import make_coherent
      try:
        make_coherent(buffer)
      except RuntimeError as e:   # the buffer stays where it was, read the slow way
        self.copied = str(e)
      else:
        # the linked program holds the old address; tinygrad links again on the next call, as after free_intermediates
        small.warp_enqueue.captured.__dict__.pop("linear", None)
    mapped = np.frombuffer(buffer.as_memoryview(allow_zero_copy=True), dtype=np.uint8)
    expected = int(np.prod(shape))
    if buffer.nbytes != expected or mapped.nbytes < expected:
      raise RuntimeError(f"the warp's output is {buffer.nbytes} bytes ({mapped.nbytes} mapped), not {expected}")
    self._mapped = mapped[:expected]
    # coherent, the link sends from the GPU's buffer; otherwise from host memory it is read into after each frame
    self._host = None if self.copied is None else np.empty(expected, dtype=np.uint8)
    self.output = memoryview(self._mapped if self._host is None else self._host)
    for frame in frames:
      frame.fill(PROBE)
    self.start(*pointers, eye, eye)
    self.wait()
    # the frames go with this call; their cache entries must not outlive them
    for key in zip(small.vision_input_names, pointers, strict=True):
      small._blob_cache.pop(key, None)
    if not (np.frombuffer(self.output, dtype=np.uint8) == PROBE).all():
      raise RuntimeError("the small model's warp output is not read live: the link would send a stale frame")

  def start(self, frame: int, big_frame: int, tfm, big_tfm) -> None:
    """Warp the camera buffers at these addresses under their transforms; `output` holds it once wait() returns."""
    self._small.warp_frames((frame, big_frame), tfm, big_tfm)

  def wait(self) -> None:
    self._device.synchronize()
    if self._host is not None:
      np.copyto(self._host, self._mapped)
