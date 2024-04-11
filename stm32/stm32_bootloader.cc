/*
 * stm32_bl.c
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */

#include "stm32/stm32_bootloader.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/unistd.h>



#include "stm32_impl.hh"
#include "stm32/stm32.hh"
#include "stm32/stm32_bl.h"
#include "utils_misc/int_macros.h"
#include "debug/dbg.h"
#include <debug/log.h>

#ifdef CONFIG_STM32_DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "stm32.bootloader"


void Stm32_Bootloader::stm32Bl_sendStart(void) {
  const unsigned char c = STM32_INIT;
  m_stm32_uart.stm32_write_bl(&c, 1);
}

void Stm32_Bootloader::stm32Bl_sendCommand(stm32_cmd_T cmd) {

  D(db_logi(logtag, "%s(0x%x)", __func__, cmd));
  const unsigned char buf[2] = { cmd, invCmd(cmd) };
  m_stm32_uart.stm32_write_bl(buf, sizeof buf);
}

void Stm32_Bootloader::stm32Bl_sendAddress(uint32_t addr) {
  unsigned char buf[5] = { GET_BYTE_3(addr), GET_BYTE_2(addr), GET_BYTE_1(addr), GET_BYTE_0(addr)};
  buf[4] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
  m_stm32_uart.stm32_write_bl(buf, sizeof buf);
}

int Stm32_Bootloader::stm32Bl_recv(unsigned char *buf, int buf_size, int wait_ms) {
  LockGuard lock(stm32_read_mutex);
#define WFR_TOTAL_MS wait_ms
#define WFR_INTERVAL_MS 50
  if (!buf) {
    return -1;
  }

  int n = 0;
  DD(db_logi(logtag, "stm32Bl: wait for response\n"));
  for (int i = 0; i < (WFR_TOTAL_MS / WFR_INTERVAL_MS); ++i) {
    m_stm32_uart.task_delay(WFR_INTERVAL_MS);
    DD(db_printf(":"));
    n += m_stm32_uart.stm32_read_bl(buf + n, buf_size - 1 - n);
    if (n > 0) {
      DD(db_logi(logtag, "stm32Bl: %d bytes received\n", n));
      return n;
    }

  }
  D(db_logi(logtag, "stm32Bl: stop wait for response\n"));
  return 0;

}


bool  Stm32_Bootloader::stm32Bl_expect(stm32_cmd_T cmd, int wait_ms) {
  unsigned char buf[16];
  buf[0] = '\0';
  if (int n = stm32Bl_recv(buf, sizeof buf, wait_ms); n != 1) {
    db_loge(logtag, "%s failed. Timeout %d ms reached or too much data (%d)", __func__, wait_ms, n);
    return false;
  }
  if (buf[0] != cmd) {
    db_loge(logtag, "%s failed. Expected 0x%x. Got 0x%x", __func__, cmd, buf[0]);
    return false;
  }
  return true;
}

bool Stm32_Bootloader::stm32Bl_expect_ack() {
  return stm32Bl_expect(STM32_ACK, 100);
}


bool Stm32_Bootloader::stm32Bl_doStart(void) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);

  stm32Bl_sendStart();
  if (!stm32Bl_expect_ack()) {
    return false;
  }
  return true;
}

void Stm32_Bootloader::stm32Bl_getId(void) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  unsigned char buf[16];
  stm32Bl_sendCommand(STM32_GID);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (n) {
    D(db_logi(logtag, "stm32Bl_getId(): %d bytes received\n", n));
    for (int i=0; i < n; ++i) {
      db_logi(logtag, "%d: 0x%x\n", i, (unsigned)buf[i]);
    }
  }

}

void Stm32_Bootloader::stm32Bl_get(void) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  unsigned char buf[16];
  stm32Bl_sendCommand(STM32_GET);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (n) {
    D(db_logi(logtag, "stm32Bl_get(): %d bytes received\n", n));
    for (int i=0; i < n; ++i) {
      db_logi(logtag, "%d: 0x%x\n", i, (unsigned)buf[i]);
    }
  }

}

#define FLASH_PAGE_SIZE 1024
#define FLASH_START_ADDRESS 0x8000000

bool Stm32_Bootloader::stm32Bl_doWriteMemory(uint32_t dst_addr, unsigned char *data, unsigned data_len) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  unsigned char buf[16];

  stm32Bl_sendCommand(STM32_WR);
  if (!stm32Bl_expect_ack()) {
    db_loge(logtag, "%s failed. No STM32_ACK after STM32_WR received", __func__);
    return false;
  }

  stm32Bl_sendAddress(dst_addr);
  if (!stm32Bl_expect_ack()) {
    db_loge(logtag, "%s failed. No STM32_ACK after dst_addr received", __func__);
    return false;
  }

  buf[0] = data_len - 1;
  m_stm32_uart.stm32_write_bl(buf, 1);
  uint8_t chksum = buf[0];
  for (int i = 0; i < data_len; ++i) {
    chksum ^= data[i];
  }
  m_stm32_uart.stm32_write_bl(data, data_len);
  m_stm32_uart.stm32_write_bl(&chksum, 1);

  if (1 != stm32Bl_recv(buf, sizeof buf, 20000) || buf[0] != STM32_ACK) {
    db_loge(logtag, "%s failed. No STM32_ACK after data received", __func__);
    return false;
  }
  return true;
}

bool  Stm32_Bootloader::stm32Bl_doEraseFlash(int start_page, uint8_t page_count) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  unsigned char buf[16];

  stm32Bl_sendCommand(STM32_ERASE);
  if (!stm32Bl_expect_ack()) {
    db_loge(logtag, "%s failed. No STM32_ACK after STM32_ERASE received", __func__);
    return false;
  }

  buf[0] = page_count - 1;
  m_stm32_uart.stm32_write_bl(buf, 1);
  uint8_t chksum = buf[0];

  for (int i=0; i < page_count; ++i) {
    uint8_t c = start_page + i;
    chksum ^= c;
    m_stm32_uart.stm32_write_bl(&c, 1);
  }
  m_stm32_uart.stm32_write_bl(&chksum, 1);

  db_logi(logtag, "waiting for erase to complete\n");
  if (1 != stm32Bl_recv(buf, sizeof buf, 60000) || buf[0] != STM32_ACK) {
    db_loge(logtag, "%s failed. No STM32_ACK received", __func__);
    return false;
  }

  return true;

}

bool  Stm32_Bootloader::stm32Bl_doExtEraseFlash(uint16_t start_page, uint16_t page_count) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  unsigned char buf[16];
  stm32Bl_sendCommand(STM32_ERASEN);
  int n = stm32Bl_recv(buf, sizeof buf, 100);
  if (!stm32Bl_expect_ack()) {
    db_loge(logtag, "%s failed. No STM32_ACK after STM32_ERASEN received", __func__);
    return false;
  }

  buf[0] = GET_BYTE_1(page_count-1);
  buf[1] = GET_BYTE_0(page_count-1);
  m_stm32_uart.stm32_write_bl(buf, 2);

  uint8_t chksum = buf[0] ^ buf[1];
  for (uint16_t i=0; i < page_count; ++i) {
    uint16_t pn = start_page + i;
    buf[0] = GET_BYTE_1(pn);
    buf[1] = GET_BYTE_0(pn);
    m_stm32_uart.stm32_write_bl(buf, 2);

    chksum ^= buf[0];
    chksum ^= buf[1];
  }
  m_stm32_uart.stm32_write_bl(&chksum, 1);

  n = stm32Bl_recv(buf, 1, 60000);
  if (!(n == 1 && buf[0] == STM32_ACK)) {
    db_loge(logtag, "%s failed. No STM32_ACK received", __func__);
    return false;
  }

  return true;

}


bool  Stm32_Bootloader::stm32Bl_eraseFlashByFileSize(uint32_t startAddr, size_t size) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  uint32_t endAddr = startAddr + size;

  unsigned startPage = (startAddr - FLASH_START_ADDRESS) / FLASH_PAGE_SIZE;
  unsigned endPage = ((endAddr -  FLASH_START_ADDRESS) + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE;
  unsigned nPages = endPage - startPage;

  D(db_logi(logtag, "startPage:%d, endPage:%d, nPages:%d\n", startPage, endPage, nPages));
  return stm32Bl_doEraseFlash(startPage, nPages);
  return false;
}


bool  Stm32_Bootloader::stm32Bl_writeMemoryFromBinFile(const char *srcFile, uint32_t addr) {
  LockGuard lock_read(stm32_read_mutex);
  LockGuard lock_write(stm32_write_mutex);
  struct stat statBuf;
  unsigned char buf[256];
  unsigned bytesWritten = 0;
  unsigned file_size = 0;
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
      db_loge(logtag, "fstat(2) failed\n");
      perror(0);
    }
    close(fd);
  } else {
    db_loge(logtag, "open(2) failed\n");
    perror(0);
  }
  db_logi(logtag, "bytes-written:%u\n", bytesWritten);
  return bytesWritten == file_size;
}


