#include "utils_misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "debug/dbg.h"
#include "assert.h"
#include <stdlib.h>

MutexRecursive::MutexRecursive() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}
#include <esp_log.h>
void MutexRecursive::lock() {
  //ESP_LOGI("trace", "lock()");
  if (xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), portMAX_DELAY))
    return;
  abort();
}

bool MutexRecursive::try_lock() {
  return !!xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), 0);
}

void MutexRecursive::unlock() {
  //ESP_LOGI("trace", "unlock()");
  xSemaphoreGiveRecursive(static_cast<QueueHandle_t>(mHandle));
}



