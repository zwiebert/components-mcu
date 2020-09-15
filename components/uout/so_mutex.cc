#include <misc/mutex.hh>
#include <app/config/proj_app_cfg.h>

#ifdef USE_MUTEX
RecMutex so_mutex;
#else
DummyMutex so_mutex;
#endif
