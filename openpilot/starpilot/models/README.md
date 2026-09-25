# Driving models

Model Manager installs the versioned catalog into `/data/models/v26`. Downloads use the catalog's declared size and SHA-256, publish atomically, and share progress and cancellation between Galaxy and the native menus. Small and Chestnut selections take effect when modeld next starts. Randomization chooses only installed, verified models for the available hardware and respects exclusions; favorites are a separate browsing preference.

Catalog behavior versions describe model inputs and outputs. The artifact ABI and compiler revision separately identify runtime compatibility. Existing compiled artifacts can be migrated when their kernels and state can be preserved and verified; changing the version label alone does not establish compatibility. Unavailable catalog entries cannot be downloaded or selected.

`modeld` records the loaded artifact digest, process identity, variant, and fallback reason in a bounded volatile receipt. Status requires a matching running process and fresh valid model outputs before reporting an active model. A failed Chestnut load falls back to the initialized small runner. Artifact integrity and bench inference do not establish on-road driving quality.

## Model Laboratory

Laboratory artifact management uses separate AMD variants of small models. A standard on-device artifact or a Chestnut-class large model cannot substitute for one. The manifest declares each variant under `accelerator_artifacts.amd`, with `execution_device: "AMD"`, the current `compiler_revision`, and the same artifact format, digest, size, and chunk-count fields as ordinary catalog artifacts. Files are named `<id>_driving_amd_tinygrad.pkl` under the model's existing catalog directory. Deleting an AMD variant preserves the ordinary on-device model.

The Galaxy page and parked configuration owner are available. The standalone pair runner verifies both AMD artifacts, shares compatible camera preprocessing, keeps each model's state separate, and rejects the whole frame if either role fails. It decodes each model's own action units before choosing lateral curvature and longitudinal acceleration/stopping. The compositor assigns lateral geometry and the corresponding plan columns to the lateral model, and leads, scene metadata, and current-frame pose to the longitudinal model.

Paired inference is not integrated into modeld yet, so production reports `runtimeSupported: false` and rejects enabling a pair. Completing it requires two verified AMD small-model variants, modeld loading/fallback and fresh pair status, then a Chestnut bench run covering real shared-warp execution, memory, timing and driver monitoring coexistence. CPU state-isolation and mocked frame tests do not qualify the AMD pair.
