/**
 * \file stm32_bl.cc
 * \brief access STM32F103 bootloader
 * \author: bertw
 */

#include <stm32/stm32_bootloader.hh>

extern Stm32_Bootloader stm32_bootloader;

void stm32Bl_sendStart(void) { stm32_bootloader.stm32Bl_sendStart(); }
/// \brief        send a command to STM32 (\link stm32_cmd_T \endlin)
/// \param cmd    command to send
void stm32Bl_sendCommand(stm32_cmd_T cmd) { stm32_bootloader.stm32Bl_sendCommand(cmd); }
/// \brief        send an address
/// \param addr   the address to send
void stm32Bl_sendAddress(uint32_t addr) { stm32_bootloader.stm32Bl_sendAddress(addr); }

/**
 * \brief              receive data from STM32
 * \param buf,buf_size destination buffer
 * \wait_ms            timout
 * \return             number of bytes received
 */
int stm32Bl_recv(unsigned char *buf, int buf_size, int wait_ms) { return stm32_bootloader.stm32Bl_recv(buf, buf_size, wait_ms); }

/// \brief send \link STM32_INIT \endlink and check for \link STM32_ACK \endlink
/// \return true if ACK was received
bool stm32Bl_doStart(void) { return stm32_bootloader.stm32Bl_doStart(); }
/// \brief send \link STM32_GID \endlink and print result (debug)
void stm32Bl_getId(void) { stm32_bootloader.stm32Bl_getId(); }
/// \brief send \link STM32_GET \endlink and print result (debug)
void stm32Bl_get(void) { stm32_bootloader.stm32Bl_get(); }

/*
 * \brief erase Flash-ROM on STM32
 */
bool stm32Bl_doEraseFlash(int start_page, uint8_t page_count) {return stm32_bootloader.stm32Bl_doEraseFlash(start_page, page_count); }

/*
 * \brief write to memory on STM32
 * \param start_page  start address for erasing as page number
 * \param page_count  size for erasing as page count
 */
bool stm32Bl_doWriteMemory(uint32_t dst_addr, unsigned char *data, unsigned data_len) { return stm32_bootloader.stm32Bl_doWriteMemory(dst_addr, data, data_len); }

/*
 * \brief copy file to memory on STM32
 */
bool stm32Bl_writeMemoryFromBinFile(const char *srcFile, uint32_t addr) { return stm32_bootloader.stm32Bl_writeMemoryFromBinFile(srcFile, addr); }

/*
 * \brief           erase FLASH-ROM on STM32
 * \param addr      start for erasing as byte address
 * \param size      size for erasing as bytes count
 */
bool stm32Bl_eraseFlashByFileSize(uint32_t addr, unsigned size) { return stm32_bootloader.stm32Bl_eraseFlashByFileSize(addr, size); }
