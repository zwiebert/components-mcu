/**
 * \file        stm32/mutex.h
 * \brief       thread mutex to protect USART read and write access
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

  bool stm32_read_mutexTake();
  bool stm32_read_mutexTakeTry();
  void stm32_read_mutexGive();

  bool stm32_write_mutexTake();
  bool stm32_write_mutexTakeTry();
  void stm32_write_mutexGive();

#ifdef __cplusplus
  }
#endif

