#ifdef __cplusplus
  extern "C" {
#endif
/*
 * periodic.h
 *
 *  Created on: 24.03.2020
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"

// periodic timers return true if time elapsed
// *state should contain zero at first call
bool periodic_ts(unsigned interval_ts, unsigned *state);
bool periodic(unsigned interval, unsigned *state);


#ifdef __cplusplus
  }
#endif
