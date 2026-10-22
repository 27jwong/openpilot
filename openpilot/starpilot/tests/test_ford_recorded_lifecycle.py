"""Route0a13721d7a3cb316/00000024--f79244cffe: Mach-E stock ACC, flags3, AE32.

Captured configuration and event order; synthetic CAN/perception/USB transport.
No VIN, location or native permission is injected. Physical acceptance is separate.
"""

import pytest
from opendbc.car.ford.values import CAR
from openpilot.starpilot.tests.ford_lifecycle_helpers import ford_joined as ford_joined, run_actual_producer


@pytest.mark.parametrize('identity', [(CAR.FORD_MUSTANG_MACH_E_MK1, 18)])
@pytest.mark.parametrize('alpha', [False])
@pytest.mark.parametrize('hands_free', [False, True])
def test_route24_accelerator_at_engagement_full_brake_stock_off_fault_reverse_and_main_off(ford_joined, monkeypatch):
  run_actual_producer(ford_joined, monkeypatch, scenario='route24')
