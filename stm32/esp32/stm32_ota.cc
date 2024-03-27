/*
 * stm32_ota.c
 *
 *  Created on: 11.06.2019
 *      Author: bertw
 */

#include "stm32/stm32_ota.h"
#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "utils_misc/int_macros.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "net/http_client.h"
#include "debug/dbg.h"
#include "esp_log.h"
#include <utils_misc/cstring_utils.hh>

#ifdef CONFIG_STM32_DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32.ota"


#define STM32_WAIT_AFTER_BOOT_MS 2000

static stm32ota_state_T state;

stm32ota_state_T stm32ota_getState(void) { return state; }

bool stm32Ota_firmwareUpdate(const char *file_name) {
  LockGuard lock(stm32_mutex);
  bool result = false;

  D(ESP_LOGI(logtag, "boot STM32 into bootloader..."));
  stm32_runBootLoader();
  vTaskDelay(STM32_WAIT_AFTER_BOOT_MS / portTICK_PERIOD_MS);

  D(ESP_LOGI(logtag, "connect to bootloader..."));
  if (!stm32Bl_doStart()) {
    ESP_LOGE(logtag, "connecting to bootloader failed");
    goto done;
  }

  D(ESP_LOGI(logtag, "erase flash and write firmware..."));
  if (!stm32Bl_writeMemoryFromBinFile(file_name, 0x8000000)) {
    ESP_LOGE(logtag, "erase flash and writing firmware failed");
    goto done;
  }

  result = true;

  done:
  D(ESP_LOGI(logtag, "boot STM32 into firmware..."));
  stm32_runFirmware();
  vTaskDelay(STM32_WAIT_AFTER_BOOT_MS / portTICK_PERIOD_MS);

  return result;
}

bool stm32Ota_firmwareDownload(const char *url, const char *file_name) {
  D(ESP_LOGI(logtag, "download file name=<%s> url=<%s>\n", file_name, url));
  return httpClient_downloadFile(url, file_name);
}

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
  stm32_runFirmware();
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
