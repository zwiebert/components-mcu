/*
 * run_time.h
 *
 *  Created on: 27.03.2020
 *      Author: bertw
 */

#ifndef MISC_TIME_INCLUDE_MISC_TIME_RUN_TIME_H_
#define MISC_TIME_INCLUDE_MISC_TIME_RUN_TIME_H_

#include <stdint.h>

#ifndef USE_ESP_GET_TIME
extern volatile uint32_t run_time_s_;
extern volatile uint32_t run_time_ts_;
#define run_time_ts() (run_time_ts_ + 0)
#define run_time_s() (run_time_s_ + 0)
#elif defined TEST_HOST
uint32_t run_time_ts();
uint32_t run_time_s();
#else
#include <esp_timer.h>
#define run_time_s() (esp_timer_get_time() / 1000000)
#define run_time_ts() (esp_timer_get_time() / 100000)
#endif

#define get_now_time_ts run_time_ts
#define get_now_time_s run_time_s

#endif /* MISC_TIME_INCLUDE_MISC_TIME_RUN_TIME_H_ */
