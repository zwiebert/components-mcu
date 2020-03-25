/*
 * periodic.h
 *
 *  Created on: 24.03.2020
 *      Author: bertw
 */

#ifndef MISC_TIME_INCLUDE_MISC_TIME_PERIODIC_H_
#define MISC_TIME_INCLUDE_MISC_TIME_PERIODIC_H_

#include "stdbool.h"

// periodic timers return true if time elapsed
// *state should contain zero at first call
bool periodic_ts(unsigned interval_ts, unsigned *state);
bool periodic(unsigned interval, unsigned *state);


#endif /* MISC_TIME_INCLUDE_MISC_TIME_PERIODIC_H_ */
