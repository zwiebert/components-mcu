/**
 * \file stm32/stm32_bl.h
 * \brief access STM32F103 bootloader
 * \author: bertw
 */

#pragma once

#include <stdbool.h>
#include "utils_misc/int_types.h"


#define   STM32_INIT 0x7F

enum stm32_cmd_T : uint8_t {
  STM32_ACK = 0x79,
  STM32_ERASE = 0x43,
  STM32_ERASEN = 0x44,
  STM32_GET = 0x00,
  STM32_GVR = 0x01,
  STM32_GID = 0x02,
  STM32_NACK = 0x1F,
  STM32_RD = 0x11,
  STM32_RP = 0x82,
  STM32_RP_NS = 0x83,
  STM32_RUN = 0x21,
  STM32_UNPCTWR = 0x73,
  STM32_UR = 0x92,
  STM32_UR_NS = 0x93,
  STM32_UW_NS = 0x74,
  STM32_WP = 0x63,
  STM32_WP_NS = 0x64,
  STM32_WR = 0x31

};

/// \brief        send \link STM32_INIT \endlink
void stm32Bl_sendStart(void);
/// \brief        send a command to STM32 (\link stm32_cmd_T \endlin)
/// \param cmd    command to send
void stm32Bl_sendCommand(stm32_cmd_T cmd);
/// \brief        send an address
/// \param addr   the address to send
void stm32Bl_sendAddress(uint32_t addr);

/**
 * \brief              receive data from STM32
 * \param buf,buf_size destination buffer
 * \wait_ms            timout
 * \return             number of bytes received
 */
int stm32Bl_recv(char *buf, int buf_size, int wait_ms);

/// \brief send \link STM32_INIT \endlink and check for \link STM32_ACK \endlink
/// \return true if ACK was received
bool stm32Bl_doStart(void);
/// \brief send \link STM32_GID \endlink and print result (debug)
void stm32Bl_getId(void);
/// \brief send \link STM32_GET \endlink and print result (debug)
void stm32Bl_get(void);

/*
 * \brief erase Flash-ROM on STM32
 */
bool stm32Bl_doEraseFlash(int start_page, uint8_t page_count);

/*
 * \brief write to memory on STM32
 * \param start_page  start address for erasing as page number
 * \param page_count  size for erasing as page count
 */
bool stm32Bl_doWriteMemory(uint32_t dst_addr, char *data, unsigned data_len);

/*
 * \brief copy file to memory on STM32
 */
bool stm32Bl_writeMemoryFromBinFile(const char *srcFile, uint32_t addr);

/*
 * \brief           erase FLASH-ROM on STM32
 * \param addr      start for erasing as byte address
 * \param size      size for erasing as bytes count
 */
bool stm32Bl_eraseFlashByFileSize(uint32_t addr, unsigned size);
