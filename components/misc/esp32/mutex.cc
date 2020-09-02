#include "misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "debug/debug.h"
#include "assert.h"

RecMutex::RecMutex() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}

void RecMutex::lock() {
  if (xSemaphoreTakeRecursive(mHandle, portMAX_DELAY))
    return;
  assert(false);
}

bool RecMutex::tryLock() {
  return !!xSemaphoreTakeRecursive(mHandle, 0);
}

void RecMutex::unlock() {
  xSemaphoreGiveRecursive(mHandle);
}



