/**
 * \file        stm32/mutex.hh
 * \brief       thread mutex to protect USART read and write access
 */

#pragma once

#include "utils_misc/mutex.hh"

extern RecMutex stm32_read_mutex, stm32_write_mutex;
