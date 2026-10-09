import math
from types import SimpleNamespace

import pytest

from cereal import car, custom, log
from opendbc.car.car_helpers import interfaces
from opendbc.car.hyundai.values import CAR as HYUNDAI_CAR
from opendbc.car.mazda.values import CAR as MAZDA_CAR
from opendbc.car.vehicle_model import VehicleModel
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.modeld.constants import ModelConstants

from openpilot.starpilot.controls.lib.neural_network_feedforward import (
  DEFAULT_NNFF_LAT_JERK_FRICTION_FACTOR,
  NNFF_MODELS_PATH,
  PALISADE_NNFF_LAT_JERK_FRICTION_FACTOR,
  FluxModel,
  LatControlNNFF,
  get_friction_torque,
  get_nnff_lat_jerk_friction_factor,
)


def test_palisade_nnff_jerk_friction_factor_is_damped_for_bumps():
  assert get_nnff_lat_jerk_friction_factor(HYUNDAI_CAR.HYUNDAI_PALISADE_2023) == PALISADE_NNFF_LAT_JERK_FRICTION_FACTOR
  assert PALISADE_NNFF_LAT_JERK_FRICTION_FACTOR < DEFAULT_NNFF_LAT_JERK_FRICTION_FACTOR


def test_other_nnff_cars_keep_default_jerk_friction_factor():
  assert get_nnff_lat_jerk_friction_factor(HYUNDAI_CAR.HYUNDAI_SONATA) == DEFAULT_NNFF_LAT_JERK_FRICTION_FACTOR
  assert get_nnff_lat_jerk_friction_factor(HYUNDAI_CAR.HYUNDAI_PALISADE) == DEFAULT_NNFF_LAT_JERK_FRICTION_FACTOR


def test_friction_torque_is_in_torque_units():
  # unlike opendbc's get_friction, latAccelFactor must not scale it
  torque_params = SimpleNamespace(friction=0.2, latAccelFactor=2.5)
  assert get_friction_torque(1.0, 0.0, 0.3, torque_params) == pytest.approx(0.2)
  assert get_friction_torque(-1.0, 0.0, 0.3, torque_params) == pytest.approx(-0.2)
  assert get_friction_torque(0.15, 0.0, 0.3, torque_params) == pytest.approx(0.1)
  assert get_friction_torque(0.05, 0.1, 0.3, torque_params) == 0.0


STRAIGHT_PLAN = [0.0] * ModelConstants.IDX_N


def _build_cx30_nnff(friction):
  CarInterface = interfaces[MAZDA_CAR.MAZDA_CX_30]
  CP = CarInterface.get_non_essential_params(MAZDA_CAR.MAZDA_CX_30)
  CI = CarInterface(CP, custom.StarPilotCarParams.new_message())
  controller = LatControlNNFF(CP.as_reader(), CI, DT_CTRL)
  controller.update_live_torque_params(CP.lateralTuning.torque.latAccelFactor, 0.0, friction)
  VM = VehicleModel(CP)

  CS = car.CarState.new_message()
  CS.vEgo = 20.0
  CS.steeringPressed = False
  CS.steeringAngleDeg = 0.0
  CS.steeringRateDeg = 0.0

  params = log.LiveParametersData.new_message()
  params.steerRatio = CP.steerRatio
  params.stiffnessFactor = 1.0
  params.roll = 0.0
  params.angleOffsetDeg = 0.0
  return controller, VM, CS, params


def _model_data(lateral_accels):
  model_data = log.ModelDataV2.new_message()
  model_data.orientation.x = [0.0] * ModelConstants.IDX_N
  model_data.orientation.y = [0.0] * ModelConstants.IDX_N
  model_data.acceleration.y = lateral_accels
  return model_data


def _feedforward(controller, VM, CS, params, desired_lateral_accel, model_data, toggles):
  desired_curvature = desired_lateral_accel / CS.vEgo ** 2
  _, _, pid_log = controller.update(True, CS, VM, params, False, desired_curvature, False, 0.3, None, model_data, toggles)
  return pid_log.f, pid_log.error


@pytest.mark.parametrize("model_data", [None, _model_data(STRAIGHT_PLAN)], ids=["no_model", "straight_plan"])
@pytest.mark.parametrize("desired_lateral_accel", [1.0, -1.0])
def test_nnff_lite_feedforward_includes_learned_friction(model_data, desired_lateral_accel):
  # wheel straight, so the error is the full request, well past the friction threshold
  toggles = SimpleNamespace(nnff=False, nnff_lite=True)
  ff = {}
  for friction in (0.0, 0.25):
    controller, VM, CS, params = _build_cx30_nnff(friction)
    ff[friction], _ = _feedforward(controller, VM, CS, params, desired_lateral_accel, model_data, toggles)
  assert ff[0.25] - ff[0.0] == pytest.approx(math.copysign(0.25, desired_lateral_accel))


def test_nnff_off_feedforward_includes_learned_friction():
  toggles = SimpleNamespace(nnff=False, nnff_lite=False)
  ff = {}
  for friction in (0.0, 0.25):
    controller, VM, CS, params = _build_cx30_nnff(friction)
    ff[friction], _ = _feedforward(controller, VM, CS, params, 1.0, None, toggles)
  assert ff[0.25] - ff[0.0] == pytest.approx(0.25)


def test_lat_accel_friction_factor_does_not_stick_after_a_straight():
  controller, VM, CS, params = _build_cx30_nnff(0.2)
  factor = controller.lat_accel_friction_factor
  _feedforward(controller, VM, CS, params, 0.0, _model_data(STRAIGHT_PLAN), SimpleNamespace(nnff=False, nnff_lite=True))
  assert controller.lat_accel_friction_factor == factor < 1.0


@pytest.mark.parametrize("desired_lateral_accel", [1.0, -1.0])
def test_nn_friction_override_adds_friction_to_feedforward(desired_lateral_accel):
  # straight plan ahead, so the future inputs don't flip sign against the current request
  toggles = SimpleNamespace(nnff=True, nnff_lite=False)
  plan = _model_data([desired_lateral_accel] * ModelConstants.IDX_N)
  results = {}
  for override in (False, True):
    for friction in (0.0, 0.25):
      controller, VM, CS, params = _build_cx30_nnff(friction)
      controller.lat_torque_nn_model = FluxModel(NNFF_MODELS_PATH / "MAZDA_CX9_2021.json")
      controller.nnff_loaded = True
      controller.nn_friction_override = override
      results[(override, friction)] = _feedforward(controller, VM, CS, params, desired_lateral_accel, plan, toggles)

  # override off: friction doesn't touch the NN path
  assert results[(False, 0.25)] == pytest.approx(results[(False, 0.0)])
  # override on with no friction learned: no constant offset sneaks in (it used to add siglin(0))
  assert results[(True, 0.0)] == pytest.approx(results[(False, 0.0)])
  # override on: friction lands in the feedforward and the PID error is left alone
  ff_on, error_on = results[(True, 0.25)]
  ff_off, error_off = results[(False, 0.25)]
  assert ff_on - ff_off == pytest.approx(math.copysign(0.25, desired_lateral_accel))
  assert error_on == pytest.approx(error_off)
