/**
 * \file        stm32/stm32_uart.hh
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include <stm32/stm32.h>
#include <stm32/stm32_uart.hh>

#include <stdbool.h>
#include <stdint.h>

#include <hal/uart_types.h>
#include "driver/uart.h"

#include "stm32/mutex.hh"

class Stm32_Uart_ESP32: public Stm32_Uart {

public:
/// \brief  Restart STM32 into bootloader
  void stm32_runBootLoader(void) override;

/// \brief Restart STM32 into firmware
  void stm32_runFirmware(void) override;

/// \brief Restart STM32
  void stm32_reset(void) override;

/// \brief Check if the STM32 is currently running the firmware
  bool stm32_isFirmwareRunning(void) override;

  void task_delay(unsigned ms) override {
    vTaskDelay(ms / portTICK_PERIOD_MS);
  }

private:
  /// write data to stm32
  int p_stm32_write(const char *data, unsigned data_len) override;
  /// read data from stm32
  int p_stm32_read(char *buf, unsigned buf_size) override;
  /// read line from stm32 with waiting for Queue outside of stm32_mutex
  int p_stm32_read_line(char *buf, unsigned buf_size, unsigned wait_ms) override;
  /// get char from STM32 (with optional blocking)
  int p_stm32_getc(bool block) override;
  /// write data to STM32 bootloader
  int p_stm32_write_bl(const char *data, unsigned data_len) override;
  /// read data from STM32 bootloader
  int p_stm32_read_bl(char *buf, unsigned buf_size) override;

public:
  typedef enum {
    STM32_MODE_NONE, STM32_MODE_FIRMWARE, STM32_MODE_BOOTLOADER
  } stm32_mode_T;

  enum my_uart_event_T {
    MY_UART_WAKE_UP = UART_EVENT_MAX
  };

  void stm32_configSerial(stm32_mode_T mode);
  void stm32_wake_up_queue();
  void stm32_setup(const cfg_stm32 *stm32_cfg);
private:
  uart_port_t m_uart = UART_NUM_1;
  stm32_mode_T stm32_mode = STM32_MODE_NONE;
  QueueHandle_t uart0_queue;
public:
  cfg_stm32 stm32_config;
};

