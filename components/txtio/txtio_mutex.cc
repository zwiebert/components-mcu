#include "txtio_mutex.h"
#include <misc/mutex.hh>

RecMutex txtio_mutex;

void txtio_mutexSetup() {
}

bool txtio_mutexTake() {
  txtio_mutex.lock();
  return true;
}

bool txtio_mutexTakeTry() {
  return txtio_mutex.tryLock();
}

void txtio_mutexGive() {
  txtio_mutex.unlock();
}


