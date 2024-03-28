/*
 * stm32_ota.c
 *
 *  Created on: 11.06.2019
 *      Author: bertw
 */

#include "stm32/stm32_ota.h"
#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "stm32_impl.hh"
#include "stm32/lock_guard.hh"
#include "utils_misc/int_macros.h"
#include "net/http_client.h"
#include "debug/dbg.h"
#include "debug/log.h"
#include <utils_misc/cstring_utils.hh>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef CONFIG_STM32_DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32.ota"


static stm32ota_state_T state;

stm32ota_state_T stm32ota_getState(void) { return state; }

struct task_parm {
  csu url;
};

void stm32ota_doUpdate_task(void *pvParameter) {
  state = stm32ota_RUN;
  auto parm = static_cast<task_parm *>(pvParameter);

  if (stm32Ota_firmwareDownload(parm->url, STM32_FW_FILE_NAME)) {
    if (stm32Ota_firmwareUpdate(STM32_FW_FILE_NAME)) {
      state = stm32ota_DONE;
      goto done;
    }
  }

  state = stm32ota_FAIL;

  done:
  delete(parm);
  stm32_uart->stm32_runFirmware();
  vTaskDelete(NULL);
}

bool stm32ota_doUpdate(const char *firmware_url) {
  if (state == stm32ota_RUN)
    return false;

  auto parm = new task_parm;
  parm->url = firmware_url;

  xTaskCreate(&stm32ota_doUpdate_task, "stm32ota_task", 16384, parm, 5, NULL);
  return false;
}
