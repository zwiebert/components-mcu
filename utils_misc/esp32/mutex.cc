#include "utils_misc/mutex.hh"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <esp_log.h>

#include "debug/dbg.h"
#include "assert.h"
#include <stdlib.h>

#ifdef CONFIG_UTILS_MISC_MUTEX_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "utils_misc.mutex"

MutexRecursive::MutexRecursive() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}

void MutexRecursive::lock() {
  D(ESP_LOGE("mutex", "lock() from %s", pcTaskGetName(nullptr)));
  if (xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), portMAX_DELAY))
    return;
  abort();
}

bool MutexRecursive::try_lock() {
  D(ESP_LOGE("mutex", "try_lock() from %s", pcTaskGetName(nullptr)));
  return !!xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), 0);
}

void MutexRecursive::unlock() {
  D(ESP_LOGE("mutex", "unlock() from %s", pcTaskGetName(nullptr)));
  xSemaphoreGiveRecursive(static_cast<QueueHandle_t>(mHandle));
}


MutexRecursive_with_logging::MutexRecursive_with_logging() {
  mHandle = xSemaphoreCreateRecursiveMutex();
  assert(mHandle);
}

void MutexRecursive_with_logging::lock() {
  ESP_LOGE("mutex", "lock() from %s", pcTaskGetName(nullptr));
  if (xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), portMAX_DELAY))
    return;
  abort();
}

bool MutexRecursive_with_logging::try_lock() {
  ESP_LOGE("mutex", "try_lock() from %s", pcTaskGetName(nullptr));
  return !!xSemaphoreTakeRecursive(static_cast<QueueHandle_t>(mHandle), 0);
}

void MutexRecursive_with_logging::unlock() {
  ESP_LOGE("mutex", "unlock() from %s", pcTaskGetName(nullptr));
  xSemaphoreGiveRecursive(static_cast<QueueHandle_t>(mHandle));
}



void MutexDummy_with_logging::lock() {
  ESP_LOGE("mutex", "lock() from %s", pcTaskGetName(nullptr));
}

bool MutexDummy_with_logging::try_lock() {
  ESP_LOGE("mutex", "try_lock() from %s", pcTaskGetName(nullptr));
  return true;
}

void MutexDummy_with_logging::unlock() {
  ESP_LOGE("mutex", "unlock() from %s", pcTaskGetName(nullptr));
}


