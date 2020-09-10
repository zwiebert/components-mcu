#include "misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "debug/dbg.h"
#include "assert.h"
#include <stdlib.h>

RecursiveMutex::RecursiveMutex() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}
#include <esp_log.h>
void RecursiveMutex::lock() {
  //ESP_LOGI("trace", "lock()");
  if (xSemaphoreTakeRecursive(mHandle, portMAX_DELAY))
    return;
  abort();
}

bool RecursiveMutex::try_lock() {
  return !!xSemaphoreTakeRecursive(mHandle, 0);
}

void RecursiveMutex::unlock() {
  //ESP_LOGI("trace", "unlock()");
  xSemaphoreGiveRecursive(mHandle);
}



