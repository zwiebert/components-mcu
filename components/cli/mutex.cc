#include <cli/mutex.h>
#include <misc/mutex.hh>

static RecMutex cli_mutex;

void mutex_setup() {
}

bool mutex_cliTake() {
  cli_mutex.lock();
  return true;
}

bool mutex_cliTakeTry() {
  return cli_mutex.tryLock();
}

void mutex_cliGive() {
  cli_mutex.unlock();
}
