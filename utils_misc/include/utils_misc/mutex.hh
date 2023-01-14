
#pragma once

class MutexRecursive {
public:
  MutexRecursive();
  void lock();
  bool try_lock();
  void unlock();

private:
  void *mHandle;
};

class MutexDummy {
public:
  inline void lock() {
  }
  inline bool try_lock() {
    return true;
  }
  inline void unlock() {
  }
};

#include <mutex>

#ifdef CONFIG_APP_USE_MUTEX
using RecMutex = MutexRecursive;
#else
using RecMutex = MutexDummy;
#endif

using LockGuard = std::lock_guard<RecMutex>;
