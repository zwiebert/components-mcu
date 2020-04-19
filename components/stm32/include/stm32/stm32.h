/*
 * uart.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

int stm32_write(const char *data, unsigned data_len);
int stm32_read(char *buf, unsigned buf_size);
int stm32_write_bl(const char *data, unsigned data_len);
int stm32_read_bl(char *buf, unsigned buf_size);

void stm32_runBootLoader(void);
void stm32_runFirmware(void);
void stm32_reset(void);

bool stm32_isFirmwareRunning(void);

struct cfg_stm32 {
  uint8_t uart_tx_gpio, uart_rx_gpio;
  uint8_t boot_gpio_is_inverse:1;
  uint8_t boot_gpio:7;
  uint8_t reset_gpio;

};

extern struct cfg_stm32 *stm32_cfg;

void stm32_setup(const struct cfg_stm32 *cfg_stm32);

