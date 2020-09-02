#pragma once

#include "locking_ptr.hh"

class RecMutex {
public:
  RecMutex();
  bool lock();
  bool tryLock();
  void unlock();

private:
  void *mHandle;
};

class DummyMutex {
public:
  inline bool lock() { return true; }
  inline bool tryLock() { return true; }
  inline void unlock() {}
};


template<class mutex>
class MutexLocker {
public:
  MutexLocker(mutex &lock) :
      mMutex(&lock) {
    mIsLocked = mMutex->lock();
  }
  ~MutexLocker() {
    if (mMutex)
    mMutex->unlock();
  }
  MutexLocker(MutexLocker<mutex> &&other) {
    mMutex = other.mMutex;
    mIsLocked = other.mIsLocked;
    other.mMutex = nullptr;
  }

  MutexLocker& operator =(MutexLocker<mutex> const &&other) {
    mMutex = other.mMutex;
    mIsLocked = other.mIsLocked;
    other.mMutex = nullptr;
  }

  operator bool() const { return mIsLocked; }
private:
  MutexLocker(MutexLocker<mutex> const &other) = delete;
  MutexLocker& operator =(MutexLocker<mutex> const &other) = delete;
private:
  mutex *mMutex;
  bool mIsLocked;
};

template <class T> using mutLp =  locking_ptr<T, RecMutex>;

using ThreadLock =  MutexLocker<RecMutex>;
