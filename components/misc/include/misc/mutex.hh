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

