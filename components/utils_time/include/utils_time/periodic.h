#ifdef __cplusplus
  extern "C" {
#endif

/**
 * \file     misc_time/periodic.h
 * \brief    functions to poll for periodic intervals
 *
 */


#pragma once

#include "stdbool.h"

/**
 * \brief                returns periodically true after interval
 * \note                 state is reset when returning true and next interval starts from then
 * \param interval_ts    interval in s/10
 * \param state          pointer to a static variable to store state. Should contain 0 at first call
 * \return               returns true after interval_ts has elapsed
 */
bool periodic_ts(unsigned interval_ts, unsigned *state);

/**
 * \brief                returns periodically true after interval
 * \note                 state is reset when returning true and next interval starts from then
 * \param interval       interval in s
 * \param state          pointer to a static variable to store state. Should contain 0 at first call
 * \return               returns true after interval_ts has elapsed
 */
bool periodic(unsigned interval, unsigned *state);


#ifdef __cplusplus
  }
#endif
