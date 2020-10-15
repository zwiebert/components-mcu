#include <utils_misc/mutex.hh>
#include <app_config/proj_app_cfg.h>

#ifdef USE_MUTEX
RecMutex so_mutex;
#else
MutexDummy so_mutex;
#endif
