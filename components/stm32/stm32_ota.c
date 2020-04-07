/*
 * stm32_ota.c
 *
 *  Created on: 11.06.2019
 *      Author: bertw
 */

#include "stm32/stm32_ota.h"
#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "misc/int_macros.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "net/http_client.h"
#include "debug/debug.h"

#define STM32_WAIT_AFTER_BOOT_MS 2000

bool stm32Ota_firmwareUpdate(const char *file_name) {
  db_printf("boot STM32 into bootloader...\n");
  stm32_runBootLoader();
  vTaskDelay(STM32_WAIT_AFTER_BOOT_MS / portTICK_PERIOD_MS);

  db_printf("connect to bootloader...\n");
  if (!stm32Bl_doStart()) {
    return false;
  }
  db_printf("erase flash and write firmware...\n");
  if (!stm32Bl_writeMemoryFromBinFile(file_name, 0x8000000)) {
    return false;
  }

  db_printf("boot STM32 into firmware...\n");
  stm32_runFirmware();
  vTaskDelay(STM32_WAIT_AFTER_BOOT_MS / portTICK_PERIOD_MS);

  return true;
}

bool stm32Ota_firmwareDownload(const char *url, const char *file_name) {
  return httpClient_downloadFile(url, file_name);
}
