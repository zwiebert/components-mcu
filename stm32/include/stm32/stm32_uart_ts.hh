/**
 * \file        stm32/stm32_uart.hh
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include "stm32_uart.hh"
#include "stm32/mutex.hh"
#include <stm32/lock_guard.hh>

#include <stdint.h>


class Stm32_Uart_Ts: public Stm32_Uart_if {
  Stm32_Uart &m_stm32_uart;
public:
  Stm32_Uart_Ts(Stm32_Uart &stm32_uart) :
      m_stm32_uart(stm32_uart) {
  }
public:
/// write data to stm32
  int stm32_write(const char *src, unsigned src_len) override {
    LockGuard lock(stm32_write_mutex);
    return m_stm32_uart.stm32_write(src, src_len);
  }
/// read data from stm32
  int stm32_read(char *dst, unsigned dst_size) override {
    LockGuard lock(stm32_read_mutex);
    return m_stm32_uart.stm32_read(dst, dst_size);
  }
/// read line from stm32 with waiting for Queue outside of stm32_mutex
  int stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms) override {
    return m_stm32_uart.stm32_read_line(dst, dst_size, wait_ms);
  }
/// get char from STM32 (with optional blocking)
  int stm32_getc(bool block) override {
    LockGuard lock(stm32_read_mutex);
    return m_stm32_uart.stm32_getc(block);
  }
/// write data to STM32 bootloader
  int stm32_write_bl(const char *src, unsigned src_len) override {
    LockGuard lock(stm32_write_mutex);
    return m_stm32_uart.stm32_write_bl(src, src_len);
  }
/// read src_len from STM32 bootloader
  int stm32_read_bl(char *dst, unsigned dst_size) override {
    LockGuard lock(stm32_read_mutex);
    return m_stm32_uart.stm32_read_bl(dst, dst_size);
  }

  /// \brief  Restart STM32 into bootloader
  void stm32_runBootLoader(void) override {
    m_stm32_uart.stm32_runBootLoader();
  }

  /// \brief Restart STM32 into firmware
  void stm32_runFirmware(void) override {
    m_stm32_uart.stm32_runFirmware();
  }

  /// \brief Restart STM32
  void stm32_reset(void) override {
    m_stm32_uart.stm32_reset();
  }

  /// \brief Check if the STM32 is currently running the firmware
  bool stm32_isFirmwareRunning(void) override {
    return m_stm32_uart.stm32_isFirmwareRunning();
  }

  void task_delay(unsigned ms) override {
    m_stm32_uart.task_delay(ms);
  }
};

