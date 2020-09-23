#ifdef __cplusplus
  extern "C++" {
#endif
/*
 * spiffs_posix.h
 *
 *  Created on: 30.03.2020
 *      Author: bertw
 */

#pragma once

#ifndef TEST_HOST
#include "spiffs_fs.h"

#define lseek(fd, pos, whence) SPIFFS_lseek(&fs, fd, pos, whence)
#define read(fd, buf, size) SPIFFS_read(&fs, fd, buf, size)
#define write(fd, buf, size) SPIFFS_write(&fs, fd, buf, size)
#define close(fd) SPIFFS_close(&fs, fd);
#define open(name, flags, mode) SPIFFS_open(&fs, name, flags, mode)
#define open(name, flags, mode) SPIFFS_open(&fs, name, flags, mode)
#define unlink(name) SPIFFS_remove(&fs, name)

#define errno SPIFFS_errno

#define SEEK_SET SPIFFS_SEEK_SET
#define SEEK_CUR SPIFFS_SEEK_CUR
#define SEEK_END SPIFFS_SEEK_END
#define O_RDONLY SPIFFS_O_RDONLY
#define O_WRONLY SPIFFS_O_WRONLY
#define O_RDWR SPIFFS_O_RDWR
#define O_CREAT SPIFFS_O_CREAT
#define O_TRUNC SPIFFS_O_TRUNC



#else
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
  }
#endif
