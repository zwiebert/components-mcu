#ifdef __cplusplus
  extern "C" {
#endif
/*
 * mutex.h
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#pragma once

#include <app_config/proj_app_cfg.h>

#include "stdbool.h"

#ifdef USE_CLI_MUTEX
bool mutex_cliTake();
bool mutex_cliTakeTry();
void mutex_cliGive();
void mutex_setup();
#else
#define mutex_cliTake() true
#define mutex_cliTryTake() true
#define mutex_cliGive()
#define mutex_setup()
#endif

#ifdef __cplusplus
  }
#endif
