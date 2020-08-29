#ifdef __cplusplus
  extern "C" {
#endif
/*
 * stm32_ota.h
 *
 *  Created on: 11.06.2019
 *      Author: bertw
 */

#pragma once

#include <stdbool.h>

#define STM32_FW_FILE_NAME "/spiffs/rv.bin"

typedef enum { stm32ota_NONE, stm32ota_RUN, stm32ota_FAIL, stm32ota_DONE} stm32ota_state_T;
stm32ota_state_T stm32ota_getState(void);

bool stm32Ota_firmwareUpdate(const char *file_name);
bool stm32Ota_firmwareDownload(const char *url, const char *file_name);

bool stm32ota_doUpdate(const char *firmware_url);

#ifdef __cplusplus
  }
#endif
