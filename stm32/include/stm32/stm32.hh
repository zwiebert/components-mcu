/**
 * \file        stm32/stm32.hh
 * \brief       access STM32 from ESP32 for communication and firware updat
 * \author      bertw
 */

#pragma once

#include "stm32.h"
#include <stdbool.h>
#include <stdint.h>



#include "./stm32_uart.hh"
#include "utils_misc/mutex.hh"

extern RecMutex stm32_mutex;
extern Stm32_Uart_if *stm32_uart;
