"""Exact manual profiles consume a fresh physical reverse signal on local bus0."""
import pytest
from opendbc.car import structs
from opendbc.safety.tests.libsafety import libsafety_py
from opendbc.safety.tests.test_ford_full_engagement import ford_native as ford_native, select
from opendbc.safety.tests.test_ford_alpha_aol import tx

WORDS = (8, 9, 10, 11, 12, 13, 18, 19, 32, 33, 66, 67)


@pytest.mark.parametrize('base', WORDS)
def test_manual_activation_brake_reverse_fault_recovery_and_long_bounds(ford_native, base):
  n = ford_native
  select(n, base | 256)
  n.pump(40, main=3)
  n.pump(10, main=4, mask=3)
  assert n.safety.aol_get_permission_mask() == (3 if base & 1 else 1)
  assert tx(n, active=True) == bool(base & 1)
  n.rx('EngBrakeData', {'BpedDrvAppl_D_Actl': 2, 'CcStat_D_Actl': 3})
  assert n.safety.aol_get_permission_mask() == 1
  assert not tx(n, active=True)
  n.pump(20, gear=1, mask=3)
  assert n.safety.aol_get_permission_mask() == 0
  n.pump(20, gear=3, mask=3)
  assert n.safety.aol_get_permission_mask() == 1
  n.pump(20, eps=1, mask=3)
  assert n.safety.aol_get_permission_mask() == 0
  n.pump(20, eps=0, mask=3)
  assert n.safety.aol_get_permission_mask() == 1
  n.pump(20, eps=2, mask=3)
  assert n.safety.aol_get_permission_mask() == 0
  n.pump(20, eps=0, mask=3)
  assert n.safety.aol_get_permission_mask() == 0
  assert not n.safety.get_controls_allowed() and not tx(n, active=True)
  n.pump(20, main=0, mask=3)
  assert n.safety.aol_get_permission_mask() == 0


@pytest.mark.parametrize('base', WORDS)
@pytest.mark.parametrize('failure', ['missing', 'wrong-bus', 'wrong-length', 'automatic-gear'])
def test_manual_reverse_source_cannot_be_substituted(ford_native, base, failure):
  n = ford_native
  select(n, base | 256)
  real_rx = n.rx

  def filtered(name, values):
    if name != 'BCM_Lamp_Stat_FD1':
      return real_rx(name, values)
    if failure == 'missing':
      return None
    if failure == 'automatic-gear':
      return real_rx('PowertrainData_10', {'TrnRng_D_Rq': 3})
    addr, data, _ = n.packer.make_can_msg(name, 0, values)
    n.safety.safety_rx_hook(libsafety_py.make_CANPacket(addr, 1 if failure == 'wrong-bus' else 0, data if failure == 'wrong-bus' else data[:-1]))
  n.rx = filtered
  n.pump(40, main=3)
  n.pump(10, main=4, mask=3)
  assert not n.safety.safety_config_valid()
  assert n.safety.aol_get_permission_mask() == 0
  assert not tx(n, active=True)


@pytest.mark.parametrize('base', WORDS)
def test_manual_reverse_source_expiry_withdraws_axes(ford_native, base):
  n = ford_native
  select(n, base | 256)
  n.pump(40, main=3)
  n.pump(10, main=4, mask=3)
  assert n.safety.aol_get_permission_mask() & 1
  original = n.rx
  n.rx = lambda name, fields: None if name == 'BCM_Lamp_Stat_FD1' else original(name, fields)
  for _ in range(120):
    n.pump(1, main=4, mask=3)
    n.safety.safety_tick()
  assert not n.safety.safety_config_valid()
  assert n.safety.aol_get_permission_mask() == 0
  assert not tx(n, active=True)


@pytest.mark.parametrize('base', WORDS)
def test_manual_unmarked_profile_preserves_ordinary_physical_long_and_brake(ford_native, base):
  n = ford_native
  select(n, base, ae=0)
  n.word |= 256  # Emit real manual sources; ordinary safety keeps its original namespace.
  n.pump(40, main=3)
  assert not n.safety.get_controls_allowed()
  assert n.safety.aol_get_permission_mask() == 0
  assert not tx(n, active=True)
  n.pump(10, main=4)
  assert n.safety.get_controls_allowed() and n.safety.safety_config_valid()
  assert n.safety.aol_get_permission_mask() == 0
  assert tx(n, active=True) == bool(base & 1)
  n.rx('EngBrakeData', {'BpedDrvAppl_D_Actl': 2, 'CcStat_D_Actl': 3})
  assert not n.safety.get_controls_allowed() and not tx(n, active=True)


def test_release_keeps_unregistered_legacy_canfd_long_parameter_denied(ford_native):
  n = ford_native
  if n.safety.set_safety_hooks(structs.CarParams.SafetyModel.allOutput, 0) == 0:
    pytest.skip('Legacy parameter3 remains DEBUG-only; RELEASE exercises the denial')
  from opendbc.safety.tests.ford_release_contract import assert_release_denial
  select(n, 3)
  assert_release_denial(n, 3)
