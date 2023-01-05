#ifdef __cplusplus
  extern "C++" {
#endif

#pragma once



#include "stdbool.h"

#ifdef CONFIG_APP_USE_MUTEX
bool cli_mutexTake();
bool cli_mutexTakeTry();
void cli_mutexGive();
void cli_mutex_setup();
#else
#define cli_mutexTake() true
#define cli_mutexTakeTry() true
#define cli_mutexGive()
#define cli_mutex_setup()
#endif

#ifdef __cplusplus
  }
#endif
