/**
 * \file        stm32.cc
 * \brief       C interface
 * \author      bertw
 */

#include <stm32/stm32.hh>
#include <stm32/stm32.h>

/// write data to stm32
extern "C" int stm32_write(const char *src, unsigned src_len) {
  return stm32_uart->stm32_write(src, src_len);
}
/// read data from stm32
extern "C" int stm32_read(char *dst, unsigned dst_size) {
  return stm32_uart->stm32_read(dst, dst_size);
}
/// read line from stm32 with waiting for Queue outside of stm32_mutex
extern "C" int stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms) {
  return stm32_uart->stm32_read_line(dst, dst_size, wait_ms);
}
/// get char from STM32 (with optional blocking)
extern "C" int stm32_getc(bool block) {
  return stm32_uart->stm32_getc(block);
}
/// write data to STM32 bootloader
extern "C" int stm32_write_bl(const char *src, unsigned src_len) {
  return stm32_uart->stm32_write_bl(src, src_len);
}
/// read data from STM32 bootloader
extern "C" int stm32_read_bl(char *dst, unsigned dst_size) {
  return stm32_uart->stm32_read_bl(dst, dst_size);
}

/// \brief  Restart STM32 into bootloader
extern "C" void stm32_runBootLoader(void) {
  stm32_uart->stm32_runBootLoader();
}

/// \brief Restart STM32 into firmware
extern "C" void stm32_runFirmware(void) {
  stm32_uart->stm32_runFirmware();
}

/// \brief Restart STM32
extern "C" void stm32_reset(void) {
  stm32_uart->stm32_reset();
}

/// \brief Check if the STM32 is currently running the firmware
extern "C" bool stm32_isFirmwareRunning(void) {
  return stm32_uart->stm32_isFirmwareRunning();
}
