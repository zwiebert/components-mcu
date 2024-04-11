/**
 * \file stm32/stm32_bootloader.hh
 * \brief access STM32F103 bootloader
 * \author: bertw
 */

#pragma once

#include "stm32_bl.h"
#include "stm32_uart.hh"
#include <stddef.h>

class Stm32_Bootloader {

private:
  Stm32_Uart_if &m_stm32_uart;

public:
Stm32_Bootloader(Stm32_Uart_if &stm32_uart): m_stm32_uart(stm32_uart) {
}

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
  int stm32Bl_recv(unsigned char *buf, int buf_size, int wait_ms);

  /// \brief send \link STM32_INIT \endlink and check for \link STM32_ACK \endlink
  /// \return true if ACK was received
  bool stm32Bl_doStart(void);
  /// \brief send \link STM32_GID \endlink and print result (debug)
  void stm32Bl_getId(void);
  /// \brief send \link STM32_GET \endlink and print result (debug)
  void stm32Bl_get(void);

  bool stm32Bl_expect(stm32_cmd_T cmd, int wait_ms);
  bool stm32Bl_expect_ack();

  /*
   * \brief erase Flash-ROM on STM32
   */
  bool stm32Bl_doEraseFlash(int start_page, uint8_t page_count);

  /*
   * \brief write to memory on STM32
   * \param start_page  start address for erasing as page number
   * \param page_count  size for erasing as page count
   */
  bool stm32Bl_doWriteMemory(uint32_t dst_addr, unsigned char *data, unsigned data_len);

  /*
   * \brief copy file to memory on STM32
   */
  bool stm32Bl_writeMemoryFromBinFile(const char *srcFile, uint32_t addr);

  /*
   * \brief           erase FLASH-ROM on STM32
   * \param addr      start for erasing as byte address
   * \param size      size for erasing as bytes count
   */
  bool stm32Bl_eraseFlashByFileSize(uint32_t addr, size_t size);

private:
  bool stm32Bl_doExtEraseFlash(uint16_t start_page, uint16_t page_count);


};
