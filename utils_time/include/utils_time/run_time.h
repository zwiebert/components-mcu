/**
 * \file   utils_time/run_time.h
 * \brief  provide time since MCU startup
 */

#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

#include <stdint.h>

#if defined TEST_HOST
  uint32_t run_time_ts();
  uint32_t run_time_s();

#elif defined CONFIG_APP_USE_ESP_GET_TIME
#include <esp_timer.h>
#define run_time_s() (esp_timer_get_time() / 1000000)
#define run_time_ts() (esp_timer_get_time() / 100000)

#else
extern volatile uint32_t run_time_s_;
extern volatile uint32_t run_time_ts_;
#define run_time_ts() (run_time_ts_ + 0)
#define run_time_s() (run_time_s_ + 0)
#error
#endif


/// \brief get s/10 since MCU startup
#define get_now_time_ts() run_time_ts()

/// \brief get seconds since MCU startup
#define get_now_time_s() run_time_s()

#ifdef __cplusplus
  }
#endif
