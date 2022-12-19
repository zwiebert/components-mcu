#include <cli/mutex.h>
#include <utils_misc/mutex.hh>

RecMutex cli_mutex;

#ifdef CONFIG_APP_USE_MUTEX
void cli_mutex_setup() {
}

bool cli_mutexTake() {
  cli_mutex.lock();
  return true;
}

bool cli_mutexTakeTry() {
  return cli_mutex.try_lock();
}

void cli_mutexGive() {
  cli_mutex.unlock();
}
#endif
