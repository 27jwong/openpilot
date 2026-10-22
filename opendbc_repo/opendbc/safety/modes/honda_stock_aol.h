#pragma once

// Keep each flag update atomic across CAN/USB interrupts while compacting
// private storage. No policy, source checks, or deadlines change.
static uint16_t honda_stock_aol_flags = 0U;
_Static_assert(__atomic_always_lock_free(sizeof(honda_stock_aol_flags), &honda_stock_aol_flags), "Honda flag atomicity");
enum {
  HONDA_STOCK_ENABLED = 1U << 0U,
  HONDA_STOCK_TOKEN = 1U << 1U,
  HONDA_STOCK_PENDING = 1U << 2U,
  HONDA_STOCK_MAIN_NEUTRAL = 1U << 3U,
  HONDA_STOCK_LKAS_NEUTRAL = 1U << 4U,
  HONDA_STOCK_LKAS_PREVIOUS = 1U << 5U,
  HONDA_STOCK_SESSION = 1U << 6U,
  HONDA_STOCK_REQUEST_SEEN = 1U << 7U,
  HONDA_STOCK_ALT_MAIN = 1U << 8U,
  HONDA_STOCK_DRIVE = 1U << 9U,
  HONDA_STOCK_TWN = 1U << 10U,
  HONDA_STOCK_TJA = 1U << 11U,
  HONDA_STOCK_ALT_LONG = 1U << 12U,
};

static bool honda_stock_aol_flag(uint16_t bit) {
  return (__atomic_load_n(&honda_stock_aol_flags, __ATOMIC_RELAXED) & bit) != 0U;
}

static void honda_stock_aol_set_flag(uint16_t bit, bool value) {
  if (value) {
    __atomic_fetch_or(&honda_stock_aol_flags, bit, __ATOMIC_RELAXED);
  } else {
    __atomic_fetch_and(&honda_stock_aol_flags, (uint16_t)~bit, __ATOMIC_RELAXED);
  }
}

static uint32_t honda_stock_aol_grant_ts = 0U;
static uint8_t honda_stock_aol_eps = 0U;
static uint32_t honda_stock_aol_main_ts = 0U;
static RxCheck *honda_stock_aol_eps_check = NULL;
static RxCheck honda_rx_workspace[8];

// EPS and gear entries are always adjacent; derive the second pointer.
static RxCheck *honda_stock_aol_gear_check(void) {
  return (honda_stock_aol_eps_check != NULL) ? honda_stock_aol_eps_check + 1 : NULL;
}

static void honda_stock_aol_clear(void) {
  honda_stock_aol_set_flag(HONDA_STOCK_TOKEN, false);
  honda_stock_aol_set_flag(HONDA_STOCK_PENDING, false);
  honda_stock_aol_set_flag(HONDA_STOCK_MAIN_NEUTRAL, false);
  honda_stock_aol_set_flag(HONDA_STOCK_LKAS_NEUTRAL, false);
}

// A new physical grant may precede the host engagement heartbeat. It grants no output
// until an engaged heartbeat and a fresh host request acknowledge this gesture.
static void honda_stock_aol_grant(void) {
  honda_stock_aol_set_flag(HONDA_STOCK_TOKEN, true);
  honda_stock_aol_set_flag(HONDA_STOCK_PENDING, !heartbeat_engaged);
  honda_stock_aol_grant_ts = microsecond_timer_get();
  honda_stock_aol_set_flag(HONDA_STOCK_SESSION, heartbeat_engaged);
}

static void honda_stock_aol_reset(void) {
  honda_stock_aol_clear();
  honda_stock_aol_set_flag(HONDA_STOCK_ENABLED, false);
  honda_stock_aol_set_flag(HONDA_STOCK_SESSION, false);
  honda_stock_aol_set_flag(HONDA_STOCK_REQUEST_SEEN, false);
  honda_stock_aol_set_flag(HONDA_STOCK_ALT_MAIN, false);
  honda_stock_aol_set_flag(HONDA_STOCK_LKAS_PREVIOUS, false);
  honda_stock_aol_set_flag(HONDA_STOCK_DRIVE, false);
  honda_stock_aol_set_flag(HONDA_STOCK_TWN, false);
  honda_stock_aol_set_flag(HONDA_STOCK_TJA, false);
  honda_stock_aol_set_flag(HONDA_STOCK_ALT_LONG, false);
  honda_stock_aol_eps = 0U;
  honda_stock_aol_main_ts = 0U;
  honda_stock_aol_eps_check = NULL;
}

static void honda_stock_aol_host_request(uint8_t mask) {
  if (mask != 0U) { honda_stock_aol_set_flag(HONDA_STOCK_REQUEST_SEEN, true); }
}

static bool honda_stock_aol_sources_current(void) {
  bool current = false;
  if ((honda_stock_aol_eps_check != NULL) && (honda_stock_aol_gear_check() != NULL)) {
    const uint32_t now = microsecond_timer_get();
    current = honda_stock_aol_eps_check->status.msg_seen && honda_stock_aol_gear_check()->status.msg_seen &&
      (safety_get_ts_elapsed(now, honda_stock_aol_eps_check->status.last_timestamp) <= 300000U) &&
      (safety_get_ts_elapsed(now, honda_stock_aol_gear_check()->status.last_timestamp) <= 300000U) &&
      (safety_get_ts_elapsed(now, honda_stock_aol_main_ts) <= 300000U) &&
      (!honda_interceptor || (honda_interceptor_sensor_seen && honda_interceptor_sensor_valid &&
       (safety_get_ts_elapsed(now, honda_interceptor_sensor_us) <= 200000U)));
  }
  return current;
}

static uint8_t honda_stock_aol_request_mask(void) {
  uint8_t request = 0U;
  const uint32_t now = microsecond_timer_get();
  const uint32_t grant_age = safety_get_ts_elapsed(now, honda_stock_aol_grant_ts);
  const bool pending_expired = honda_stock_aol_flag(HONDA_STOCK_PENDING) && (grant_age > AOL_HOST_REQUEST_TIMEOUT_US);
  const bool expired = honda_stock_aol_flag(HONDA_STOCK_REQUEST_SEEN) &&
    (safety_get_ts_elapsed(now, aol_host_request_ts) > AOL_HOST_REQUEST_TIMEOUT_US);
  if (relay_malfunction || expired || pending_expired || safety_rx_checks_invalid ||
      (honda_stock_aol_flag(HONDA_STOCK_SESSION) && !heartbeat_engaged) ||
      ((honda_stock_aol_flag(HONDA_STOCK_SESSION) || honda_stock_aol_flag(HONDA_STOCK_PENDING)) &&
       (!aol_rx_healthy() || !honda_stock_aol_sources_current()))) {
    honda_stock_aol_clear();
    honda_stock_aol_set_flag(HONDA_STOCK_SESSION, false);
  } else if (heartbeat_engaged && aol_rx_healthy() && honda_stock_aol_sources_current()) {
    const bool pending_ack = !honda_stock_aol_flag(HONDA_STOCK_PENDING) || ((aol_host_axis_mask != 0U) &&
      (safety_get_ts_elapsed(now, aol_host_request_ts) <= grant_age));
    if (pending_ack) {
      honda_stock_aol_set_flag(HONDA_STOCK_PENDING, false);
      honda_stock_aol_set_flag(HONDA_STOCK_SESSION, true);
      request = aol_host_axis_mask;
    }
  } else {
  }
  return request;
}

static uint8_t honda_stock_aol_permission_mask(void) {
  const uint8_t request = honda_stock_aol_request_mask();
  uint8_t permission = 0U;
  // A temporary EPS fault (2/6, e.g. a hard driver override) withdraws lateral only;
  // longitudinal stays with controls_allowed as in stock openpilot.
  const bool eps_lateral = (honda_stock_aol_eps == 0U) || (honda_stock_aol_eps == 3U) || (honda_stock_aol_eps == 4U) ||
    (honda_stock_aol_flag(HONDA_STOCK_TJA) && (honda_stock_aol_eps == 1U));
  const bool eps_known = eps_lateral || (honda_stock_aol_eps == 2U) || (honda_stock_aol_eps == 6U);
  if (acc_main_on && honda_stock_aol_flag(HONDA_STOCK_DRIVE) && eps_known) {
    if (eps_lateral && (controls_allowed || honda_stock_aol_flag(HONDA_STOCK_TOKEN))) {
      permission = request & 1U;
    }
    if (controls_allowed) {
      permission |= request & 2U;
    }
  }
  return permission;
}

static bool honda_stock_aol_selected(const CANPacket_t *msg, const RxCheck *check) {
  bool selected = false;
  if ((check != NULL) && check->status.msg_seen) {
    const CanMsgCheck *source = &check->msg[check->status.index];
    selected = msg_matches(msg, source->addr, source->bus, source->len);
  }
  return selected;
}

static void honda_stock_aol_rx(const CANPacket_t *msg, unsigned int pt_bus, bool normal_engagement) {
  if (honda_stock_aol_flag(HONDA_STOCK_ENABLED)) {
    // Retire the old session before interpreting a new validated physical edge.
    // The following gesture may create a bounded pending grant; a status read
    // must not mistake that new grant for the retired session.
    if (honda_stock_aol_flag(HONDA_STOCK_SESSION) && !heartbeat_engaged) {
      honda_stock_aol_clear();
      honda_stock_aol_set_flag(HONDA_STOCK_SESSION, false);
    }
    const unsigned int main_addr = honda_stock_aol_flag(HONDA_STOCK_ALT_MAIN) ? 0x1A6U : 0x326U;
    const unsigned int buttons_addr = honda_stock_aol_flag(HONDA_STOCK_ALT_MAIN) ? 0x1A6U : 0x296U;
    const unsigned int buttons_len = honda_stock_aol_flag(HONDA_STOCK_ALT_MAIN) ? 8U : 4U;
    if (msg_matches(msg, main_addr, pt_bus, 8U)) {
      const bool main_on = GET_BIT(msg, honda_stock_aol_flag(HONDA_STOCK_ALT_MAIN) ? 47U : 28U);
      honda_stock_aol_main_ts = microsecond_timer_get();
      if (!main_on) {
        honda_stock_aol_clear();
        honda_stock_aol_set_flag(HONDA_STOCK_MAIN_NEUTRAL, true);
      } else if (honda_stock_aol_flag(HONDA_STOCK_MAIN_NEUTRAL)) {
        honda_stock_aol_grant();
        honda_stock_aol_set_flag(HONDA_STOCK_MAIN_NEUTRAL, false);
      } else {
      }
    }
    if (msg_matches(msg, buttons_addr, pt_bus, buttons_len)) {
      const uint8_t setting = honda_stock_aol_flag(HONDA_STOCK_ALT_MAIN) ? ((msg->data[5] >> 2U) & 3U) : ((msg->data[0] >> 2U) & 3U);
      const bool lkas = setting == 1U;
      if (!lkas) {
        honda_stock_aol_set_flag(HONDA_STOCK_LKAS_NEUTRAL, true);
      } else if (honda_stock_aol_flag(HONDA_STOCK_LKAS_NEUTRAL) && !honda_stock_aol_flag(HONDA_STOCK_LKAS_PREVIOUS) && acc_main_on) {
        honda_stock_aol_grant();
        honda_stock_aol_set_flag(HONDA_STOCK_LKAS_NEUTRAL, false);
      } else {
      }
      honda_stock_aol_set_flag(HONDA_STOCK_LKAS_PREVIOUS, lkas);
    }
    if (honda_stock_aol_selected(msg, honda_stock_aol_eps_check)) {
      honda_stock_aol_eps = (msg->addr == 0x18FU) ?
        (honda_stock_aol_flag(HONDA_STOCK_TWN) ? (msg->data[5] & 15U) :
         ((GET_LEN(msg) == 6U) ? (msg->data[4] & 15U) : ((msg->data[4] >> 4U) & 15U))) :
        ((msg->data[1] >> 4U) & 15U);
      if ((honda_stock_aol_eps != 0U) && (honda_stock_aol_eps != 2U) && (honda_stock_aol_eps != 3U) &&
          (honda_stock_aol_eps != 4U) && (honda_stock_aol_eps != 6U) &&
          !(honda_stock_aol_flag(HONDA_STOCK_TJA) && (honda_stock_aol_eps == 1U))) {
        honda_stock_aol_clear();
      }
    }
    if (honda_stock_aol_selected(msg, honda_stock_aol_gear_check())) {
      uint8_t gear = msg->data[4] & 15U;
      if (msg->addr == 0x191U) {
        gear = msg->data[5] & 31U;
      } else if (msg->addr == 0x188U) {
        gear = msg->data[3] & 15U;
      } else {
      }
      honda_stock_aol_set_flag(HONDA_STOCK_DRIVE, (msg->addr == 0x188U) ? ((gear == 8U) || (gear == 0U)) :
        ((gear == 4U) || (gear == 7U) || (gear == 10U) || (gear == 11U)));
    }
    // Only a newly accepted physical cruise grant can restore an independent token.
    // Qualify after parsing this frame's brake/main state, before generic pedal checks.
    const bool engagement_source = msg_matches(msg, 0x17CU, pt_bus, 8U) ||
      msg_matches(msg, buttons_addr, pt_bus, buttons_len);
    const bool request_current = !honda_stock_aol_flag(HONDA_STOCK_REQUEST_SEEN) ||
      (safety_get_ts_elapsed(microsecond_timer_get(), aol_host_request_ts) <= AOL_HOST_REQUEST_TIMEOUT_US);
    const bool eps_known = (honda_stock_aol_eps == 0U) || (honda_stock_aol_eps == 2U) ||
      (honda_stock_aol_eps == 3U) || (honda_stock_aol_eps == 4U) || (honda_stock_aol_eps == 6U) ||
      (honda_stock_aol_flag(HONDA_STOCK_TJA) && (honda_stock_aol_eps == 1U));
    if (normal_engagement && engagement_source && controls_allowed && acc_main_on && honda_stock_aol_flag(HONDA_STOCK_DRIVE) &&
        eps_known && !brake_pressed && !regen_braking && !steering_disengage &&
        !relay_malfunction && !safety_rx_checks_invalid && request_current &&
        aol_rx_healthy() && honda_stock_aol_sources_current()) {
      honda_stock_aol_grant();
    }
  }
}

static safety_config honda_copy_rx(const RxCheck *source, unsigned int count, safety_config ret) {
  const unsigned int selected_count = (count <= 8U) ? count : 0U;
  for (unsigned int i = 0U; i < 8U; i++) {
    honda_rx_workspace[i] = (i < selected_count) ? source[i] : (RxCheck){0};
  }
  ret.rx_checks = honda_rx_workspace;
  ret.rx_checks_len = selected_count;
  return ret;
}

static safety_config honda_stock_aol_configure(uint16_t param, bool nidec, bool alt_main, unsigned int pt_bus, safety_config ret) {
  honda_stock_aol_reset();
  bool valid = nidec ? ((param == 0U) || (param == 4U) || (param == 260U) || (param == 544U) || (param == 548U) || (param == 804U)) :
    ((param == 0U) || (param == 1U) || (param == 8U) || (param == 9U) || (param == 16U) || (param == 17U) || (param == 80U) || (param == 81U) || (param == 512U) || (param == 513U));
#ifdef ALLOW_DEBUG
  valid = valid || (!nidec && ((param == 10U) || (param == 11U) || (param == 1058U) || (param == 1059U) || (param == 1570U) || (param == 1571U) || (param == 2082U) || (param == 2083U) || (param == 2211U)));
#endif
  honda_stock_aol_set_flag(HONDA_STOCK_ENABLED, valid && ((unsigned int)alternative_experience == 32U) &&
    (ret.rx_checks_len >= 3) && (ret.rx_checks_len <= 6) && (pt_bus <= 1U));
  honda_stock_aol_set_flag(HONDA_STOCK_ALT_MAIN, alt_main);
  honda_stock_aol_set_flag(HONDA_STOCK_TWN, nidec && ((param == 260U) || (param == 804U)));
  honda_stock_aol_set_flag(HONDA_STOCK_TJA, !nidec && ((param == 512U) || (param == 513U) || (param == 1570U) || (param == 1571U)));
  honda_stock_aol_set_flag(HONDA_STOCK_ALT_LONG, !nidec && ((param == 1058U) || (param == 1059U) || (param == 1570U) || (param == 1571U)));
  if (honda_stock_aol_flag(HONDA_STOCK_ENABLED) && (ret.rx_checks_len <= 6) && (pt_bus <= 1U)) {
    const unsigned int count = ret.rx_checks_len;
    static const RxCheck eps[] = {
      {.msg = {{0x18F, 0U, 7, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x18F, 0U, 6, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x190, 0U, 5, 10U, .max_counter = 3U, .ignore_quality_flag = true}}},
      {.msg = {{0x18F, 1U, 7, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x18F, 1U, 6, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x190, 1U, 5, 10U, .max_counter = 3U, .ignore_quality_flag = true}}},
    };
    static const RxCheck gear[] = {
      {.msg = {{0x191, 0U, 8, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x1A3, 0U, 8, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x188, 0U, 6, 10U, .max_counter = 3U, .ignore_quality_flag = true}}},
      {.msg = {{0x191, 1U, 8, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x1A3, 1U, 8, 10U, .max_counter = 3U, .ignore_quality_flag = true},
               {0x188, 1U, 6, 10U, .max_counter = 3U, .ignore_quality_flag = true}}},
    };
    static const RxCheck buttons[] = {
      {.msg = {{0x296, 0U, 4, 25U, .max_counter = 3U, .ignore_quality_flag = true}, {0}, {0}}},
      {.msg = {{0x296, 1U, 4, 25U, .max_counter = 3U, .ignore_quality_flag = true}, {0}, {0}}},
      {.msg = {{0x1A6, 0U, 8, 25U, .max_counter = 3U, .ignore_quality_flag = true}, {0}, {0}}},
    };
    honda_rx_workspace[0] = buttons[alt_main ? 2U : pt_bus];
    honda_rx_workspace[count] = eps[pt_bus];
    if (honda_stock_aol_flag(HONDA_STOCK_TWN) || honda_stock_aol_flag(HONDA_STOCK_TJA) || honda_stock_aol_flag(HONDA_STOCK_ALT_LONG) || (!nidec && ((param == 16U) || (param == 17U) || (param == 80U) || (param == 81U)))) {
      honda_rx_workspace[count].msg[1] = (CanMsgCheck){0};
      honda_rx_workspace[count].msg[2] = (CanMsgCheck){0};
    }
    honda_rx_workspace[count + 1U] = gear[pt_bus];
    if (honda_stock_aol_flag(HONDA_STOCK_TJA) || honda_stock_aol_flag(HONDA_STOCK_ALT_LONG) || (!nidec && ((param == 16U) || (param == 17U) || (param == 80U) || (param == 81U)))) {
      honda_rx_workspace[count + 1U].msg[2] = (CanMsgCheck){0};
    }
    honda_stock_aol_eps_check = &honda_rx_workspace[count];
    ret.rx_checks_len = count + 2U;
    static const AolSafetyPolicy policy = {
      .reset = honda_stock_aol_reset,
      .host_request = honda_stock_aol_host_request,
      .request_mask = honda_stock_aol_request_mask,
      .permission_mask = honda_stock_aol_permission_mask,
      .rx_invalid = honda_stock_aol_clear,
    };
    aol_policy = &policy;
  }
  return ret;
}
