"""Generic optional-feature admission from finalized native CarParams.

Admission creates a host; the host still checks live axis and source authority.
PCM cruise describes engagement/set-speed integration, not who controls acceleration.
"""


def display_supported(cp) -> bool:
  return bool(cp is not None and getattr(cp, 'carFingerprint', '') and
              getattr(cp, 'passive', None) is False and
              getattr(cp, 'dashcamOnly', None) is False and
              getattr(cp, 'notCar', None) is False)


def longitudinal_supported(cp) -> bool:
  return display_supported(cp) and getattr(cp, 'openpilotLongitudinalControl', None) is True
