from types import SimpleNamespace

import pytest
from openpilot.cereal import log
from openpilot.starpilot.aol.panda_faults import blocking_fault_mask, blocking_fault_list


@pytest.mark.parametrize('fault, blocks', [('interruptRateCan2', False), ('interruptRateUsb', False),
                                          ('relayMalfunction', True), ('registerDivergent', True),
                                          ('heartbeatLoopWatchdog', True)])
def test_honda_latched_interrupt_rate_faults_do_not_block(fault, blocks):
  cp = SimpleNamespace(brand='honda')
  value = log.PandaState.FaultType.schema.enumerants[fault]
  assert blocking_fault_mask(1 << value, cp) == blocks
  assert blocking_fault_list([value], cp) == blocks
  assert blocking_fault_list([log.PandaState.FaultType.interruptRateCan2, value], cp) == blocks
