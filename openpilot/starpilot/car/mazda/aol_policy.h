#pragma once

#include "selfdrive/pandad/aol_protocol.h"

// Word 0 is the stock-camera GEN1 profile; word 2 is the bare GEN2 flag, the CX-30 over its torque interceptor.
inline bool mazda_aol_param(uint16_t param) {
  return param == 0U || param == 2U;
}

inline constexpr AolSafetyProfile MAZDA_AOL_PROFILE{13U, mazda_aol_param, true};
