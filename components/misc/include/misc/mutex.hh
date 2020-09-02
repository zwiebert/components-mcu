#pragma once

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

