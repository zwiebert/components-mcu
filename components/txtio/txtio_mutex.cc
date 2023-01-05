#include <utils_misc/mutex.hh>


#ifdef CONFIG_APP_USE_MUTEX
RecMutex txtio_mutex;
#else
MutexDummy txtio_mutex;
#endif
