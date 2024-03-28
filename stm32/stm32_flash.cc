/**
 * \brief stm32_flash.hh
 * \author: bertw
 *
 * Created on: 11.06.2019
 */

#include "stm32/stm32_flash.hh"

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

#ifdef CONFIG_STM32_DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32.ota"


#define STM32_WAIT_AFTER_BOOT_MS 2000


bool stm32Ota_firmwareUpdate(const char *file_name) {
  WakeUpLockGuard lock(stm32_mutex);
  bool result = false;

  D(db_logi(logtag, "boot STM32 into bootloader..."));
  stm32_uart->stm32_runBootLoader();
  stm32_uart->task_delay(STM32_WAIT_AFTER_BOOT_MS);

  D(db_logi(logtag, "connect to bootloader..."));
  if (!stm32Bl_doStart()) {
    db_loge(logtag, "connecting to bootloader failed");
    goto done;
  }

  D(db_logi(logtag, "erase flash and write firmware..."));
  if (!stm32Bl_writeMemoryFromBinFile(file_name, 0x8000000)) {
    db_loge(logtag, "erase flash and writing firmware failed");
    goto done;
  }

  result = true;

  done:
  D(db_logi(logtag, "boot STM32 into firmware..."));
  stm32_uart->stm32_runFirmware();
  stm32_uart->task_delay(STM32_WAIT_AFTER_BOOT_MS);

  return result;
}

bool stm32Ota_firmwareDownload(const char *url, const char *file_name) {
  D(db_logi(logtag, "download file name=<%s> url=<%s>\n", file_name, url));
  return httpClient_downloadFile(url, file_name);
}


