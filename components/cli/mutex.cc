#include <cli/mutex.h>
#include <misc/mutex.hh>

RecMutex cli_mutex;

#ifdef USE_MUTEX
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
