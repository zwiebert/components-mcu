#include "misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "debug/dbg.h"
#include "assert.h"

DummyMutex dummy_mutex;

RecMutex::RecMutex() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}
#include <esp_log.h>
bool RecMutex::lock() {
  //ESP_LOGI("trace", "lock()");
  if (xSemaphoreTakeRecursive(mHandle, portMAX_DELAY))
    return true;
  assert(false);
  return false;
}

bool RecMutex::tryLock() {
  return !!xSemaphoreTakeRecursive(mHandle, 0);
}

void RecMutex::unlock() {
  //ESP_LOGI("trace", "unlock()");
  xSemaphoreGiveRecursive(mHandle);
}



