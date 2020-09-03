#pragma once

#include <app_config/proj_app_cfg.h>
#include <misc/mutex.hh>

#ifdef USE_TXTIO_MUTEX
extern RecMutex txtio_mutex;
#else
extern DummyMutex txtio_mutex;
#endif


