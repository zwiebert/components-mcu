#pragma once

class RecMutex {
public:
  RecMutex();
  void lock();
  bool tryLock();
  void unlock();

private:
  void *mHandle;
};

class DummyMutex {
public:
  inline void lock() {}
  inline bool tryLock() { return true; }
  inline void unlock() {}
};

