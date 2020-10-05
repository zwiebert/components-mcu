#pragma once

#include <app/config/proj_app_cfg.h>
#include <misc/mutex.hh>

#ifdef USE_MUTEX
extern RecMutex so_mutex;
#else
extern MutexDummy so_mutex;
#endif


