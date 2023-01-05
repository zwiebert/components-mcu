#pragma once


#include <utils_misc/mutex.hh>

#ifdef CONFIG_APP_USE_MUTEX
extern RecMutex txtio_mutex;
#else
extern MutexDummy txtio_mutex;
#endif


