#ifdef __cplusplus
  extern "C" {
#endif
/**
 * \file       stm32_ota.h
 * \brief      download and update firmware of the connected STM32
 * \author     bertw
 */

#pragma once

#include <stdbool.h>

#define STM32_FW_FILE_NAME "/spiffs/rv.bin"

typedef enum { stm32ota_NONE, stm32ota_RUN, stm32ota_FAIL, stm32ota_DONE} stm32ota_state_T;
stm32ota_state_T stm32ota_getState(void);

/// update the STM32 firmware from a FILE
bool stm32Ota_firmwareUpdate(const char *file_name);

/// just fetch the STM32 firmware from URL and store it to a FILE (FIXME: add https support)
bool stm32Ota_firmwareDownload(const char *url, const char *file_name);

/// do the complete STM32 firmware update from given URL
bool stm32ota_doUpdate(const char *firmware_url);

#ifdef __cplusplus
  }
#endif
