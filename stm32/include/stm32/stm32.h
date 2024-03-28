/**
 * \file        stm32/stm32.h
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/// stm32 component configuration
  struct cfg_stm32 {
    uint8_t uart_tx_gpio; ///< GPIO number for TX which is connected to STM32 RX
    uint8_t uart_rx_gpio; ///< GPIO number for RX which is connected to STM32 TX
    uint8_t boot_gpio_is_inverse :1; ///< invert the boot pin if connected by an inverting transistor
    uint8_t boot_gpio :7; ///< GPIO number which is connected to STM32 boot pin
    uint8_t reset_gpio;  ///< GPIO number which is connected to STM32 reset pin
  };

#ifdef __cplusplus
extern "C" {
#endif

  bool stm32_mutexTake();
  bool stm32_mutexTakeTry();
  void stm32_mutexGive();

/// write data to stm32
  int stm32_write(const char *src, unsigned src_len);
/// read data from stm32
  int stm32_read(char *dst, unsigned dst_size);
/// read line from stm32 with waiting for Queue outside of stm32_mutex
  int stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms);
/// get char from STM32 (with optional blocking)
  int stm32_getc(bool block);
/// write data to STM32 bootloader
  int stm32_write_bl(const char *src, unsigned src_len);
/// read data from STM32 bootloader
  int stm32_read_bl(char *dst, unsigned dst_size);

/// \brief  Restart STM32 into bootloader
  void stm32_runBootLoader(void);

/// \brief Restart STM32 into firmware
  void stm32_runFirmware(void);

/// \brief Restart STM32
  void stm32_reset(void);

/// \brief Check if the STM32 is currently running the firmware
  bool stm32_isFirmwareRunning(void);

  void stm32_setup(const struct cfg_stm32 *cfg_stm32);

#ifdef __cplusplus
  }
#endif

