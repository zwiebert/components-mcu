#ifdef __cplusplus
#define USE_MUTEX
#include "utils_misc/mutex.hh"
extern RecMutex stm32_mutex;
  extern "C" {
#endif


 /**
 * \file        stm32/stm32.h
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/// write data to stm32
int stm32_write(const char *data, unsigned data_len);
/// read data from stm32
int stm32_read(char *buf, unsigned buf_size);
/// get char from STM32 (with optional blocking)
int stm32_getc(bool block);
/// write data to STM32 bootloader
int stm32_write_bl(const char *data, unsigned data_len);
/// read data from STM32 bootloader
int stm32_read_bl(char *buf, unsigned buf_size);

bool stm32_mutexTake();///< lock writing to STM32 UART
bool stm32_mutexTakeTry(); ///< tryLock writing to STM32 UART
void stm32_mutexGive();  ///< unlock writing to STM32 UART

/// \brief  Restart STM32 into bootloader
void stm32_runBootLoader(void);

/// \brief Restart STM32 into firmware
void stm32_runFirmware(void);

/// \brief Restart STM32
void stm32_reset(void);

/// \brief Check if the STM32 is currently running the firmware
bool stm32_isFirmwareRunning(void);


/// stm32 component configuration
struct cfg_stm32 {
  uint8_t uart_tx_gpio; ///< GPIO number for TX which is connected to STM32 RX
  uint8_t uart_rx_gpio; ///< GPIO number for RX which is connected to STM32 TX
  uint8_t boot_gpio_is_inverse:1;///< invert the boot pin if connected by an inverting transistor
  uint8_t boot_gpio:7; ///< GPIO number which is connected to STM32 boot pin
  uint8_t reset_gpio;  ///< GPIO number which is connected to STM32 reset pin
};

/// set up the stm32 component
void stm32_setup(const struct cfg_stm32 *cfg_stm32);

#ifdef __cplusplus
  }
#endif
