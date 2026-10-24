"""Every exact composite retains its parent's RX contracts and physical gates."""
import pytest
from opendbc.car import structs
from opendbc.car.toyota.pedal_composite import PARENT_WORDS
from opendbc.safety.tests.libsafety import libsafety_py
from openpilot.starpilot.car.toyota.tests import test_rav4h_caller_native as helpers

source_matched_native = helpers.source_matched_native
libsafety_py.ffi.cdef('extern safety_config current_safety_config;')


def rx_catalog(safety):
  cfg = safety.current_safety_config
  fields = ('addr', 'bus', 'len', 'frequency', 'ignore_checksum', 'ignore_counter', 'max_counter', 'ignore_quality_flag')
  return [tuple(int(getattr(cfg.rx_checks[i].msg[0], key)) for key in fields) for i in range(cfg.rx_checks_len)]


@pytest.mark.parametrize('word,parent', sorted(PARENT_WORDS.items()))
def test_parent_catalog_and_warm_switch_are_exact(word, parent):
  safety = libsafety_py.libsafety
  safety.init_tests()
  safety.set_alternative_experience(32)
  try:
    assert safety.set_safety_hooks(structs.CarParams.SafetyModel.toyota, parent) == 0
    original = rx_catalog(safety)
    assert safety.set_safety_hooks(structs.CarParams.SafetyModel.toyota, word) == 0
    composite = rx_catalog(safety)
    assert 5 <= len(composite) <= 8
    assert composite[:len(original)] == original
    extra_eps = not any(row[0] == 0x262 and row[1] == 0 for row in original)
    extra_main = not any(row[0] in (0x1D3, 0x365) and row[1] == 0 for row in original)
    assert len(composite) == len(original) + 1 + int(extra_eps) + int(extra_main)
    assert sum(row[0] == 0x2FF for row in composite) == sum(row[0] == 0x2FF for row in original)
    assert sum(row[:3] == (0x201, 0, 6) for row in composite) == 1
    assert sum(row[0] == 0x262 and row[1] == 0 for row in composite) == 1
    assert sum(row[0] in (0x1D3, 0x365) and row[1] == 0 for row in composite) == 1
    pedal = next(row for row in composite if row[0] == 0x201)
    assert pedal[3:] == (50, 0, 0, 15, 0)
    assert not safety.safety_config_valid() and safety.aol_get_permission_mask() == 0
    # A warm switch must not retain the pedal's extra catalog or authority.
    assert safety.set_safety_hooks(structs.CarParams.SafetyModel.toyota, parent) == 0
    assert rx_catalog(safety) == original
    assert not safety.safety_config_valid() and safety.aol_get_permission_mask() == 0
  finally:
    safety.set_alternative_experience(0)
    safety.set_safety_hooks(structs.CarParams.SafetyModel.noOutput, 0)
