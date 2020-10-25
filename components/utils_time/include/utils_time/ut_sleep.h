#pragma once


/**
 * \brief       Sleep function w/o actual sleeping used in unit tests
 * \param secs  Will be added to results of any calls to \link run_time_s \endlink and \link run_time_ts \endlink
 */
void ut_sleep(unsigned secs);

/**
 * \brief        Sleep function w/o actual sleeping used in unit tests
 * \param tsecs  s/10 will be added to results of any calls to \link run_time_s \endlink and \link run_time_ts \endlink
 */
void ut_sleep_ts(unsigned tsecs);
