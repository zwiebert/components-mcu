/**
 * \file uout/so_mutex.hh
 * \brief Currently(?) unused (XXX!).  Mutex for uout.
 */
#pragma once

#include <app_config/proj_app_cfg.h>
#include <utils_misc/mutex.hh>

#ifdef USE_MUTEX
extern RecMutex so_mutex;
#else
extern MutexDummy so_mutex;
#endif


