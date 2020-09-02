#include "misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "debug/debug.h"
#include "assert.h"

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



