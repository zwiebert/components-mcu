/**
 * \file           misc_time/periodic.h
 * \brief          provide time since MCU startup
 */

#ifdef __cplusplus
  extern "C" {
#endif
/*
 * run_time.h
 *
 *  Created on: 27.03.2020
 *      Author: bertw
 */

#pragma once
#include "app_config/proj_app_cfg.h"
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


/// \brief get s/10 since MCU startup
#define get_now_time_ts() run_time_ts()

/// \brief get seconds since MCU startup
#define get_now_time_s() run_time_s()

#ifdef __cplusplus
  }
#endif
