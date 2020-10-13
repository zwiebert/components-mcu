#pragma once

#include <app/config/proj_app_cfg.h>
#include <misc/mutex.hh>

#ifdef USE_MUTEX
extern RecMutex txtio_mutex;
#else
extern MutexDummy txtio_mutex;
#endif


