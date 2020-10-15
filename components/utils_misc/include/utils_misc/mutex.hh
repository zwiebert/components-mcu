#include <app_config/proj_app_cfg.h>
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

#ifdef USE_MUTEX
using RecMutex = MutexRecursive;
#else
using RecMutex = MutexDummy;
#endif

using LockGuard = std::lock_guard<RecMutex>;

