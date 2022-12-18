#include <stm32/stm32.h>
#include <utils_misc/mutex.hh>

RecMutex stm32_mutex;

#if 0 // XXX: Disabled to prevent linker project netmcu (which has a lot of circular dependencies)
bool stm32_mutexTake() {
  stm32_mutex.lock();
  return true;
}

bool stm32_mutexTakeTry() {
  return stm32_mutex.try_lock();
}

void stm32_mutexGive() {
  stm32_mutex.unlock();
}

#endif
