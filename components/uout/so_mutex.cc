#include <misc/mutex.hh>
#include <app/config/proj_app_cfg.h>

#ifdef USE_MUTEX
RecMutex so_mutex;
#else
MutexDummy so_mutex;
#endif
