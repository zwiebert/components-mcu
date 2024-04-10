#include <stm32/stm32.h>
#include <utils_misc/mutex.hh>

#ifdef CONFIG_STM32_MAKE_COMPONENT_THREAD_SAFE
RecMutex stm32_read_mutex, stm32_write_mutex;
#else
MutexDummy stm32_read_mutex, stm32_write_mutex;
#endif

extern "C" bool stm32_read_mutexTake() {
  stm32_read_mutex.lock();
  return true;
}

extern "C" bool stm32_read_mutexTakeTry() {
  return stm32_read_mutex.try_lock();
}

extern "C" void stm32_read_mutexGive() {
  stm32_read_mutex.unlock();
}


extern "C" bool stm32_write_mutexTake() {
  stm32_write_mutex.lock();
  return true;
}

extern "C" bool stm32_write_mutexTakeTry() {
  return stm32_write_mutex.try_lock();
}

extern "C" void stm32_write_mutexGive() {
  stm32_write_mutex.unlock();
}

