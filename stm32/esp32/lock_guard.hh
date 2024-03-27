#pragma once
#include <stm32/stm32.h>

void stm32_wake_up_queue();

template<typename _Mutex>
class stm32_lock_guard {
public:
  typedef _Mutex mutex_type;

  explicit stm32_lock_guard(mutex_type &__m) :
      _M_device(__m) {
    stm32_wake_up_queue();
    _M_device.lock();
  }

  ~stm32_lock_guard() {
    _M_device.unlock();
  }

  stm32_lock_guard(const stm32_lock_guard&) = delete;
  stm32_lock_guard& operator=(const stm32_lock_guard&) = delete;

private:
  mutex_type &_M_device;
};

using WakeUpLockGuard = stm32_lock_guard<RecMutex>;
