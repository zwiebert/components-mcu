/*
 * stm32_bl.h
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */

#ifndef MAIN_STM32_BL_H_
#define MAIN_STM32_BL_H_

#include <stdbool.h>
#include "misc/int_types.h"


#define   STM32_INIT 0x7F

typedef enum {
  STM32_ACK = 0x79,
  STM32_ERASE = 0x43,
  STM32_ERASEN = 0x44,
  STM32_GET = 0x00,
  STM32_GVR = 0x01,
  STM32_GID = 0x02,
  STM32_NACK = 0x1F,
  STM32_RD = 0x11,
  STM32_RP = 0x82,
  STM32_RP_NS = 0x83,
  STM32_RUN = 0x21,
  STM32_UNPCTWR = 0x73,
  STM32_UR = 0x92,
  STM32_UR_NS = 0x93,
  STM32_UW_NS = 0x74,
  STM32_WP = 0x63,
  STM32_WP_NS = 0x64,
  STM32_WR = 0x31

} stm32_cmd_T;

void stm32Bl_sendStart(void);
void stm32Bl_sendCommand(stm32_cmd_T cmd);
void stm32Bl_sendAddress(u32 addr);
int stm32Bl_recv(char *buf, int buf_size, int wait_ms);

bool stm32Bl_doStart(void);
void stm32Bl_getId(void);
void stm32Bl_get(void);
bool stm32Bl_doEraseFlash(int start_page, u8 page_count);
bool stm32Bl_doWriteMemory(u32 dst_addr, char *data, unsigned data_len);

bool stm32Bl_writeMemoryFromBinFile(const char *srcFile, u32 addr);
bool stm32Bl_eraseFlashByFileSize(u32 addr, unsigned size);


#endif /* MAIN_STM32_BL_H_ */
