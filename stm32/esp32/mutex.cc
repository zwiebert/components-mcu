#include <stm32/stm32.h>
#include <utils_misc/mutex.hh>

#ifdef CONFIG_STM32_MAKE_COMPONENT_THREAD_SAFE
RecMutex stm32_mutex;
#else
MutexDummy stm32_mutex;
#endif

extern "C" bool stm32_mutexTake() {
  stm32_mutex.lock();
  return true;
}

extern "C" bool stm32_mutexTakeTry() {
  return stm32_mutex.try_lock();
}

extern "C" void stm32_mutexGive() {
  stm32_mutex.unlock();
}

