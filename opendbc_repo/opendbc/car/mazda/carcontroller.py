import math
import numpy as np
from opendbc.can import CANPacker
from opendbc.car import Bus, structs
from opendbc.car.common.conversions import Conversions as CV
from opendbc.car.lateral import apply_driver_steer_torque_limits
from opendbc.car.interfaces import CarControllerBase
from opendbc.car.mazda import mazdacan
from opendbc.car.mazda.values import CarControllerParams, Buttons, MazdaSafetyFlags
from openpilot.common.realtime import ControlsTimer as Timer, DT_CTRL
from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.params import Params
from openpilot.starpilot.common.experimental_state import CEStatus

VisualAlert = structs.CarControl.HUDControl.VisualAlert
LongCtrlState = structs.CarControl.Actuators.LongControlState

# crossfade duration between stock MRCC and openpilot long, ramped linearly with speed
TRANSITION_TIME_MIN = 0.5  # s at standstill, and the hard floor once the lead penalty is applied
TRANSITION_TIME_SLOPE = (3.0 - TRANSITION_TIME_MIN) / (55.0 * CV.MPH_TO_MS)  # s per m/s, so 3s at 55mph
# the crossfade shortens as the lead closes in: nothing at a 2s gap, 3s off at 0.5s or less
LEAD_PENALTY_HEADWAY = [0.5, 2.0]  # s of headway to the lead
LEAD_PENALTY = [3.0, 0.0]  # s off the crossfade
# an approaching lead latches on at the engage thresholds and only lets go at the wider
# release ones, so a lead track that flickers around a boundary cannot chatter the crossfade
LEAD_ENGAGE_HEADWAY = 0.85  # s of headway
LEAD_RELEASE_HEADWAY = 1.1  # s of headway
LEAD_ENGAGE_TTC = 8.0  # s to impact
LEAD_RELEASE_TTC = 10.0  # s to impact

class CarController(CarControllerBase):
  def __init__(self, dbc_names, CP):
    super().__init__(dbc_names, CP)
    self.apply_torque_last = 0
    self.ti_apply_torque_last = 0
    self.packer = CANPacker(dbc_names[Bus.pt])
    self.brake_counter = 0
    self.ccp = CarControllerParams(CP)
    self.hold_timer = Timer(6.0)
    self.hold_delay = Timer(.5) # delay before we start holding as to not hit the brakes too hard
    self.resume_timer = Timer(0.5)
    self.cancel_delay = Timer(0.07) # 70ms delay to try to avoid a race condition with stock system
    self.acc_filter = FirstOrderFilter(0.0, .1, DT_CTRL, initialized=False)
    self.long_active_last = False
    self.params_memory = Params(memory=True)
    self.lead_d_filter = FirstOrderFilter(0.0, .075, DT_CTRL, initialized=False)
    self.lead_v_filter = FirstOrderFilter(0.0, .075, DT_CTRL, initialized=False)
    self.lead_distance = 0.0
    self.lead_velocity = 0.0
    # factor for blending stock MRCC and openpilot long. 0 is fully stock, 1 is fully openpilot
    self.blend_coeff = 0.0
    # seconds for a full crossfade in either direction; recomputed every cycle
    self.transition_time = TRANSITION_TIME_MIN
    # latched by the engage thresholds, released by the wider ones
    self.lead_approaching = False
    self.distance_last = None



  def update(self, CC, CS, now_nanos, starpilot_toggles):
    # card surfaces radarState's leadOne on CS so opendbc stays free of messaging
    lead_status = bool(getattr(CS, "openpilot_lead_status", False))  # whether lead is valid
    if lead_status:
      self.lead_distance = self.lead_d_filter.update(float(getattr(CS, "openpilot_lead_d_rel", 0.0)))  # relative distance in meters
      self.lead_velocity = self.lead_v_filter.update(float(getattr(CS, "openpilot_lead_v_rel", 0.0)))  # relative velocity in m/s

    can_sends = []

    apply_torque = 0
    ti_apply_torque = 0

    if CC.latActive:
      # calculate steer and also set limits due to driver torque
      new_torque = int(round(CC.actuators.torque * self.ccp.STEER_MAX))
      apply_torque = apply_driver_steer_torque_limits(new_torque, self.apply_torque_last,
                                                      CS.out.steeringTorque, self.ccp)
      if self.CP.flags & MazdaSafetyFlags.TORQUE_INTERCEPTOR:
        if CS.ti_lkas_allowed:
          ti_new_torque = int(round(CC.actuators.torque * self.ccp.STEER_MAX))
          ti_apply_torque = apply_driver_steer_torque_limits(ti_new_torque, self.apply_torque_last,
                                                    CS.out.steeringTorque, self.ccp)

    self.apply_torque_last = apply_torque
    self.ti_apply_torque_last = ti_apply_torque

    if self.CP.flags & MazdaSafetyFlags.GEN1:
      if CC.cruiseControl.cancel:
        # If brake is pressed, let us wait >70ms before trying to disable crz to avoid
        # a race condition with the stock system, where the second cancel from openpilot
        # will disable the crz 'main on'. crz ctrl msg runs at 50hz. 70ms allows us to
        # read 3 messages and most likely sync state before we attempt cancel.
        self.brake_counter = self.brake_counter + 1
        if self.frame % 10 == 0 and not (CS.out.brakePressed and self.brake_counter < 7):
          # Cancel Stock ACC if it's enabled while OP is disengaged
          # Send at a rate of 10hz until we sync with stock ACC state
          can_sends.append(mazdacan.create_button_cmd(self.packer, self.CP, CS.crz_btns_counter, Buttons.CANCEL))
      else:
        self.brake_counter = 0
        if CC.cruiseControl.resume and self.frame % 5 == 0:
          # Mazda Stop and Go requires a RES button (or gas) press if the car stops more than 3 seconds
          # Send Resume button when planner wants car to move
          can_sends.append(mazdacan.create_button_cmd(self.packer, self.CP, CS.crz_btns_counter, Buttons.RESUME))

      # send HUD alerts
      if self.frame % 50 == 0:
        ldw = CC.hudControl.visualAlert == VisualAlert.ldw
        steer_required = CC.hudControl.visualAlert == VisualAlert.steerRequired
        # TODO: find a way to silence audible warnings so we can add more hud alerts
        steer_required = steer_required and CS.lkas_allowed_speed
        can_sends.append(mazdacan.create_alert_command(self.packer, CS.cam_laneinfo, ldw, steer_required))

      if self.CP.openpilotLongitudinalControl:
        hold = False
        if CS.out.standstill:
          hold = self.hold_timer.active()
        else:
          self.hold_timer.reset()

          stock_acc = CS.crz_info["ACCEL_CMD"]
          op_acc = CC.actuators.accel * 1150
          op_acc = max(-1000, min(op_acc, 1000))

          if getattr(starpilot_toggles, "blended_acc", False):
            if CC.longActive:
              if not self.long_active_last:
                self.acc_filter.initialized = False
              # Hand longitudinal to openpilot only while experimental mode is actually
              # resolved on; otherwise let the stock radar ACC command through.
              target_acc = op_acc if CC.experimentalMode else stock_acc
              raw_acc_output = self.acc_filter.update(target_acc)
            else:
              raw_acc_output = stock_acc
          else:
            raw_acc_output = op_acc

          raw_acc_output = max(-1000, min(raw_acc_output, 1000))
          CS.crz_info["ACCEL_CMD"] = raw_acc_output

        if self.frame % 2 == 0:
          can_sends.extend(mazdacan.create_radar_command(self.packer, self.frame, CC.longActive, CS, hold))

    elif self.CP.flags & MazdaSafetyFlags.GEN2:
      blended_acc = bool(getattr(starpilot_toggles, "blended_acc", False))

      if self.CP.openpilotLongitudinalControl and CC.longActive:
        stock_acc = CS.acc["ACCEL_CMD"]
        op_acc = (CC.actuators.accel * 200) + 2000

        # Force CEM with the closest distance setting
        if CS.distance_setting == 1:
          self.params_memory.put_int("CEStatus", CEStatus["USER_OVERRIDDEN"])
        elif self.distance_last == 1:
          self.params_memory.put_int("CEStatus", CEStatus["OFF"])

        if blended_acc:
          self.distance_last = CS.distance_setting

          # Built from last cycle's coefficient, so a status change lands on the next
          # cycle instead of stepping the command partway through this one.
          if self.blend_coeff > 0:
            CS.acc["ACCEL_CMD"] = (self.blend_coeff * op_acc) + ((1 - self.blend_coeff) * stock_acc)

        else:
          CS.acc["ACCEL_CMD"] = op_acc

      if blended_acc and self.CP.openpilotLongitudinalControl:
        # Tracked every cycle, engaged or not, so engaging picks up the coefficient the
        # situation calls for rather than whatever was frozen at the last disengage.
        ce_status = self.params_memory.get_int("CEStatus", default=CEStatus["OFF"])
        lead_headway = self.lead_distance / max(CS.out.vEgo, 0.1) if lead_status else math.inf

        # An approaching lead is its own reason to hand over, and blends straight to
        # openpilot long rather than switching experimental mode on. Time to impact is
        # infinite while the lead is not closing, which leaves the headway holding the
        # latch on its own instead of dropping it every time relative speed crosses zero.
        lead_ttc = self.lead_distance / -self.lead_velocity if self.lead_velocity < 0 else math.inf
        headway_thresh, ttc_thresh = ((LEAD_RELEASE_HEADWAY, LEAD_RELEASE_TTC) if self.lead_approaching else
                                      (LEAD_ENGAGE_HEADWAY, LEAD_ENGAGE_TTC))
        self.lead_approaching = bool(lead_status and (lead_headway < headway_thresh or lead_ttc < ttc_thresh))

        # Every status at or above USER_OVERRIDDEN is a reason CEM resolved experimental
        # mode on; OFF and USER_DISABLED both mean it is off.
        blend_to_op = ce_status >= CEStatus["USER_OVERRIDDEN"] or self.lead_approaching

        # blend in openpilot long
        if blend_to_op and self.blend_coeff < 1:
          self.blend_coeff += min((DT_CTRL / self.transition_time), (1 - self.blend_coeff))

        # blend back out to MRCC. The reason to hand over can drop away in one cycle, but
        # the coefficient still has to decay from wherever it got to.
        elif not blend_to_op and self.blend_coeff > 0:
          self.blend_coeff -= min((DT_CTRL / self.transition_time), self.blend_coeff)

        # ramp the crossfade with speed, then shorten it by the lead penalty so a closing
        # gap hands over faster. Never quicker than the floor, however close the lead is.
        self.transition_time = max(TRANSITION_TIME_MIN + (TRANSITION_TIME_SLOPE * CS.out.vEgo) -
                                   float(np.interp(lead_headway, LEAD_PENALTY_HEADWAY, LEAD_PENALTY)),
                                   TRANSITION_TIME_MIN)

      resume = False
      hold = False
      if Timer.interval(2): # send ACC command at 50hz
        """
        Without this hold/resum logic, the car will only stop momentarily.
        It will then start creeping forward again. This logic allows the car to
        apply the electric brake to hold the car. The hold delay also fixes a
        bug with the stock ACC where it sometimes will apply the brakes too early
        when coming to a stop.
        """
        if CS.out.standstill: # if we're stopped
          if not self.hold_delay.active(): # and we have been stopped for more than hold_delay duration. This prevents a hard brake if we aren't fully stopped.
            if ((CC.cruiseControl.resume and CC.actuators.longControlState != LongCtrlState.stopping) or
                CC.cruiseControl.override or CS.out.gasPressed or
                (CC.actuators.longControlState == LongCtrlState.starting) or CS.acc["RESUME"]): # if we are resuming or overriding, we want to release the brake
              self.resume_timer.reset() # reset the resume timer so its active
            else: # otherwise we're holding
              hold = self.hold_timer.active() # hold for 6s. This allows the electric brake to hold the car.

        else: # if we're moving
          self.hold_timer.reset() # reset the hold timer so its active when we stop
          self.hold_delay.reset() # reset the hold delay

        resume = self.resume_timer.active() # stay on for 0.5s to release the brake. This allows the car to move.
        can_sends.append(mazdacan.create_acc_cmd(self.packer, CS.acc, hold, resume))


    # send steering command
    can_sends.extend(mazdacan.create_steering_control(
      self.packer, self.CP, self.frame, apply_torque, CS.cam_lkas,
      ti_apply_torque if self.CP.flags & MazdaSafetyFlags.TORQUE_INTERCEPTOR else None))

    new_actuators = CC.actuators.as_builder()
    new_actuators.torque = apply_torque / self.ccp.STEER_MAX
    new_actuators.torqueOutputCan = apply_torque

    self.long_active_last = CC.longActive
    self.frame += 1
    Timer.tick()
    return new_actuators, can_sends