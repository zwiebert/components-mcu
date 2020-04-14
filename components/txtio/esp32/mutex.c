/*
 * mutex.c
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef USE_TXTIO_MUTEX
#include "txtio_mutex.h"
#endif

#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t mutex_handle;

void txtio_mutexSetup() {

  /* Create a mutex type semaphore. */
  mutex_handle = xSemaphoreCreateRecursiveMutex();

  assert(mutex_handle);

}

bool txtio_mutexTake() {
  if (xSemaphoreTakeRecursive(mutex_handle, portMAX_DELAY)) {
    return true;
  } else {
    return false;
  }
}

bool txtio_mutexTakeTry() {
  return 0 != xSemaphoreTakeRecursive(mutex_handle, 0);
}

void txtio_mutexGive() {
  xSemaphoreGiveRecursive(mutex_handle);
}

