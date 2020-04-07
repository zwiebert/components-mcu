/*
 * stm32_ota.h
 *
 *  Created on: 11.06.2019
 *      Author: bertw
 */

#ifndef MAIN_STM32_OTA_H_
#define MAIN_STM32_OTA_H_

#include <stdbool.h>

#define STM32_FW_FILE_NAME "/spiffs/rv.bin"

bool stm32Ota_firmwareUpdate(const char *file_name);
bool stm32Ota_firmwareDownload(const char *url, const char *file_name);

#endif /* MAIN_STM32_OTA_H_ */
