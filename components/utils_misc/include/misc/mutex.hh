#include <app/config/proj_app_cfg.h>
#pragma once

class RecursiveMutex {
public:
  RecursiveMutex();
  void lock();
  bool try_lock();
  void unlock();

private:
  void *mHandle;
};

class DummyMutex {
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
using RecMutex = RecursiveMutex;
#else
using RecMutex = DummyMutex;
#endif

using LockGuard = std::lock_guard<RecMutex>;

