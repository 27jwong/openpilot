"""Reviewed local driving-model catalog. This module never loads model artifacts."""

from dataclasses import dataclass
import json
from pathlib import Path


BUNDLED_CURRENT = "bundled-current"
GENERATION = "v26"
COMPILER_REVISION = "9d0446a4ba8a532c8b674fb6ad795af015cd9dcf"
ARTIFACT_ABI = "tinygrad_single_v1_arena"
CATALOG_PATH = Path(__file__).with_name("catalog-v26.json")


@dataclass(frozen=True)
class ModelEntry:
  model_id: str
  name: str
  source_sha256: str
  source_path: str
  compiler_revision: str
  artifact_abi: str
  selectable: bool
  version: str = "current"
  uses_external_gpu: bool = False


BUNDLED = (
  ModelEntry(
    model_id=BUNDLED_CURRENT,
    name="Bundled driving model",
    source_sha256="65a08adc31d5c456219687d99b7bf5e44d61dae2d49ea67850e76105c7248cce",
    source_path="openpilot/selfdrive/modeld/models/driving_supercombo.onnx",
    compiler_revision="9d0446a4ba8a532c8b674fb6ad795af015cd9dcf",
    artifact_abi="current-onnx-tinygrad-oob",
    selectable=True,
  ),
)

CATALOG = BUNDLED + tuple(ModelEntry(
  model_id=m["id"], name=m["name"], source_sha256="", source_path="",
  compiler_revision=COMPILER_REVISION, artifact_abi=ARTIFACT_ABI, selectable=True,
  version=m["version"], uses_external_gpu=bool(m.get("uses_external_gpu", False)),
) for m in json.loads(CATALOG_PATH.read_text())["models"])

BY_ID = {entry.model_id: entry for entry in CATALOG}


def resolve_selection(requested_id: str | None) -> ModelEntry:
  """Resolve a saved request; absence means the bundled model, never an unknown ID."""
  model_id = BUNDLED_CURRENT if requested_id is None else requested_id
  if not isinstance(model_id, str) or model_id not in BY_ID or not BY_ID[model_id].selectable:
    raise ValueError("unqualified driving-model selection")
  return BY_ID[model_id]
