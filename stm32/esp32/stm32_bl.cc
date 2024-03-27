/*
 * stm32_bl.c
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/unistd.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"



#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "utils_misc/int_macros.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "debug/dbg.h"

#ifdef CONFIG_STM32_DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32.bootloader"


void stm32Bl_sendStart(void) {
  const char c = STM32_INIT;
  stm32_write_bl(&c, 1);
}

void stm32Bl_sendCommand(stm32_cmd_T ecmd) {
  const uint8_t cmd = (uint8_t)ecmd;

  D(ESP_LOGI(logtag, "%s(0x%x)", __func__, cmd));
  const char buf[2] = { cmd, (uint8_t)~cmd };
  stm32_write_bl(buf, sizeof buf);
}

void stm32Bl_sendAddress(uint32_t addr) {
  char buf[5] = { GET_BYTE_3(addr), GET_BYTE_2(addr), GET_BYTE_1(addr), GET_BYTE_0(addr)};
  buf[4] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
  stm32_write_bl(buf, sizeof buf);
}

int stm32Bl_recv(char *buf, int buf_size, int wait_ms) {
  LockGuard lock(stm32_mutex);
#define WFR_TOTAL_MS wait_ms
#define WFR_INTERVAL_MS 50
  if (!buf) {
    return -1;
  }

  int n = 0;
  DD(ESP_LOGI(logtag, "stm32Bl: wait for response\n"));
  for (int i = 0; i < (WFR_TOTAL_MS / WFR_INTERVAL_MS); ++i) {
    vTaskDelay(WFR_INTERVAL_MS / portTICK_PERIOD_MS);
    DD(db_printf(":"));
    n += stm32_read_bl(buf + n, buf_size - 1 - n);
    if (n > 0) {
      DD(ESP_LOGI(logtag, "stm32Bl: %d bytes received\n", n));
      return n;
    }

  }
  D(ESP_LOGI(logtag, "stm32Bl: stop wait for response\n"));
  return 0;

}


bool stm32bl_expect(stm32_cmd_T cmd, int wait_ms) {
  char buf[16];
  buf[0] = '\0';
  if (int n = stm32Bl_recv(buf, sizeof buf, wait_ms); n != 1) {
    ESP_LOGE(logtag, "%s failed. Timeout %d ms reached or too much data (%d)", __func__, wait_ms, n);
    return false;
  }
  if (buf[0] != cmd) {
    ESP_LOGE(logtag, "%s failed. Expected 0x%x. Got 0x%x", __func__, cmd, buf[0]);
    return false;
  }
  return true;
}
bool stm32bl_expect_ack() {
  return stm32bl_expect(STM32_ACK, 100);
}


bool stm32Bl_doStart(void) {
  LockGuard lock(stm32_mutex);
  char buf[16];

  stm32Bl_sendStart();
  if (!stm32bl_expect_ack()) {
    return false;
  }
  return true;
}

void stm32Bl_getId(void) {
  LockGuard lock(stm32_mutex);
  char buf[16];
  stm32Bl_sendCommand(STM32_GID);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (n) {
    D(ESP_LOGI(logtag, "stm32Bl_getId(): %d bytes received\n", n));
    for (int i=0; i < n; ++i) {
      ESP_LOGI(logtag, "%d: 0x%x\n", i, (unsigned)buf[i]);
    }
  }

}

void stm32Bl_get(void) {
  LockGuard lock(stm32_mutex);
  char buf[16];
  stm32Bl_sendCommand(STM32_GET);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (n) {
    D(ESP_LOGI(logtag, "stm32Bl_get(): %d bytes received\n", n));
    for (int i=0; i < n; ++i) {
      ESP_LOGI(logtag, "%d: 0x%x\n", i, (unsigned)buf[i]);
    }
  }

}

#define FLASH_PAGE_SIZE 1024
#define FLASH_START_ADDRESS 0x8000000

bool stm32Bl_doWriteMemory(uint32_t dst_addr, char *data, unsigned data_len) {
  LockGuard lock(stm32_mutex);
  char buf[16];

  stm32Bl_sendCommand(STM32_WR);
  if (!stm32bl_expect_ack()) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK after STM32_WR received", __func__);
    return false;
  }

  stm32Bl_sendAddress(dst_addr);
  if (!stm32bl_expect_ack()) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK after dst_addr received", __func__);
    return false;
  }

  buf[0] = data_len - 1;
  stm32_write_bl(buf, 1);
  uint8_t chksum = buf[0];
  for (int i = 0; i < data_len; ++i) {
    chksum ^= data[i];
  }
  stm32_write_bl(data, data_len);
  stm32_write_bl((char*)&chksum, 1);

  if (1 != stm32Bl_recv(buf, sizeof buf, 20000) || buf[0] != STM32_ACK) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK after data received", __func__);
    return false;
  }
  return true;
}

bool stm32Bl_doEraseFlash(int start_page, uint8_t page_count) {
  LockGuard lock(stm32_mutex);
  char buf[16];

  stm32Bl_sendCommand(STM32_ERASE);
  if (!stm32bl_expect_ack()) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK after STM32_ERASE received", __func__);
    return false;
  }

  buf[0] = page_count - 1;
  stm32_write_bl(buf, 1);
  uint8_t chksum = buf[0];

  for (int i=0; i < page_count; ++i) {
    uint8_t c = start_page + i;
    chksum ^= c;
    stm32_write_bl((char*)&c, 1);
  }
  stm32_write_bl((char*)&chksum, 1);

  ESP_LOGI(logtag, "waiting for erase to complete\n");
  if (1 != stm32Bl_recv(buf, sizeof buf, 60000) || buf[0] != STM32_ACK) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK received", __func__);
    return false;
  }

  return true;

}

bool stm32Bl_doExtEraseFlash(uint16_t start_page, uint16_t page_count) {
  LockGuard lock(stm32_mutex);
  char buf[16];
  stm32Bl_sendCommand(STM32_ERASEN);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (!stm32bl_expect_ack()) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK after STM32_ERASEN received", __func__);
    return false;
  }

  buf[0] = GET_BYTE_1(page_count-1);
  buf[1] = GET_BYTE_0(page_count-1);
  stm32_write_bl(buf, 2);

  uint8_t chksum = buf[0] ^ buf[1];
  for (uint16_t i=0; i < page_count; ++i) {
    uint16_t pn = start_page + i;
    buf[0] = GET_BYTE_1(pn);
    buf[1] = GET_BYTE_0(pn);
    stm32_write_bl(buf, 2);

    chksum ^= buf[0];
    chksum ^= buf[1];
  }
  stm32_write_bl((char*)&chksum, 1);

  n = stm32Bl_recv(buf, 1, 60000);
  if (!(n == 1 && buf[0] == STM32_ACK)) {
    ESP_LOGE(logtag, "%s failed. No STM32_ACK received", __func__);
    return false;
  }

  return true;

}


bool stm32Bl_eraseFlashByFileSize(uint32_t startAddr, size_t size) {
  LockGuard lock(stm32_mutex);
  uint32_t endAddr = startAddr + size;

  unsigned startPage = (startAddr - FLASH_START_ADDRESS) / FLASH_PAGE_SIZE;
  unsigned endPage = ((endAddr -  FLASH_START_ADDRESS) + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE;
  unsigned nPages = endPage - startPage;

  D(ESP_LOGI(logtag, "startPage:%d, endPage:%d, nPages:%d\n", startPage, endPage, nPages));
  return stm32Bl_doEraseFlash(startPage, nPages);
  return false;
}


bool stm32Bl_writeMemoryFromBinFile(const char *srcFile, uint32_t addr) {
  LockGuard lock(stm32_mutex);
  struct stat statBuf;
  char buf[256];
  size_t bytesWritten = 0;
  size_t file_size = 0;
  int fd;

  if (0 <= (fd = open(srcFile, O_RDONLY))) {
    if (0 <= fstat(fd, &statBuf)) {
      file_size = statBuf.st_size;
      if (stm32Bl_eraseFlashByFileSize(FLASH_START_ADDRESS, statBuf.st_size)) {
        while (1) {
          int n = read(fd, buf, 256);
          if (n <= 0) {
            break;
          }
          if (!stm32Bl_doWriteMemory(addr, buf, n)) {
            break;
          }
          addr += n;
          bytesWritten += n;
        }
      }
    } else {
      ESP_LOGE(logtag, "fstat(2) failed\n");
      perror(0);
    }
    close(fd);
  } else {
    ESP_LOGE(logtag, "open(2) failed\n");
    perror(0);
  }
  ESP_LOGI(logtag, "bytes-written:%d\n", bytesWritten);
  return bytesWritten == file_size;
}


