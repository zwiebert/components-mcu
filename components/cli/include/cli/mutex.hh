#pragma once

#include <app_config/proj_app_cfg.h>
#include <misc/mutex.hh>

#ifdef USE_CLI_MUTEX
extern RecMutex cli_mutex;
#else
extern DummyMutex cli_mutex;
#endif
