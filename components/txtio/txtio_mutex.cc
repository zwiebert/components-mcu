#include <misc/mutex.hh>
#include <app/config/proj_app_cfg.h>

#ifdef USE_MUTEX
RecMutex txtio_mutex;
#else
DummyMutex txtio_mutex;
#endif
