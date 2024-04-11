/**
 * \file        stm32/stm32_uart.hh
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include <stdint.h>

class Stm32_Uart_if {

public:
/// write data to stm32
  virtual int stm32_write(const char *src, unsigned src_len) = 0;
/// read data from stm32
  virtual int stm32_read(char *dst, unsigned dst_size) = 0;
/// read line from stm32 with waiting for Queue outside of stm32_mutex
  virtual int stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms) = 0;
/// get char from STM32 (with optional blocking)
  virtual int stm32_getc(bool block) = 0;
/// write data to STM32 bootloader
  virtual int stm32_write_bl(const unsigned char *src, unsigned src_len) = 0;
/// read src_len from STM32 bootloader
  virtual int stm32_read_bl(unsigned char *dst, unsigned dst_size) = 0;

/// \brief  Restart STM32 into bootloader
  virtual void stm32_runBootLoader(void) = 0;

/// \brief Restart STM32 into firmware
  virtual void stm32_runFirmware(void) = 0;

/// \brief Restart STM32
  virtual void stm32_reset(void) = 0;

/// \brief Check if the STM32 is currently running the firmware
  virtual bool stm32_isFirmwareRunning(void) = 0;

  virtual void task_delay(unsigned ms) = 0;
};



class Stm32_Uart : public Stm32_Uart_if {

public:
/// write data to stm32
  int stm32_write(const char *src, unsigned src_len) override {
        return p_stm32_write(src, src_len);
  }
/// read data from stm32
  int stm32_read(char *dst, unsigned dst_size) override{
        return p_stm32_read(dst, dst_size);
  }
/// read line from stm32 with waiting for Queue outside of stm32_mutex
  int stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms)override {
    return p_stm32_read_line(dst, dst_size, wait_ms);
  }
/// get char from STM32 (with optional blocking)
  int stm32_getc(bool block)override {
        return p_stm32_getc(block);
  }
/// write data to STM32 bootloader
  int stm32_write_bl(const unsigned char *src, unsigned src_len) override{
        return p_stm32_write_bl(src, src_len);
  }
/// read src_len from STM32 bootloader
  int stm32_read_bl(unsigned char *dst, unsigned dst_size) override{
        return p_stm32_read_bl(dst, dst_size);
  }

protected:
  /// write data to stm32
    virtual int p_stm32_write(const char *src, unsigned src_len) = 0;
  /// read data from stm32
    virtual int p_stm32_read(char *dst, unsigned dst_size) = 0;
  /// read line from stm32 with waiting for Queue outside of stm32_mutex
    virtual int p_stm32_read_line(char *dst, unsigned dst_size, unsigned wait_ms) = 0;
  /// get char from STM32 (with optional blocking)
    virtual int p_stm32_getc(bool block) = 0;
  /// write data to STM32 bootloader
    virtual int p_stm32_write_bl(const unsigned char *src, unsigned src_len) = 0;
  /// read data from STM32 bootloader
    virtual int p_stm32_read_bl(unsigned char *dst, unsigned dst_size) = 0;
};
