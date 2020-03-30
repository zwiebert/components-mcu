/*
 * spiffs_posix.h
 *
 *  Created on: 30.03.2020
 *      Author: bertw
 */

#ifndef STORAGE_INCLUDE_STORAGE_SPIFFS_POSIX_H_
#define STORAGE_INCLUDE_STORAGE_SPIFFS_POSIX_H_

#include "spiffs_fs.h"

#define lseek(fd, pos, whence) SPIFFS_lseek(&fs, fd, pos, whence)
#define read(fd, buf, size) SPIFFS_read(&fs, fd, buf, size)
#define write(fd, buf, size) SPIFFS_write(&fs, fd, buf, size)
#define close(fd) SPIFFS_close(&fs, fd);
#define open(name, flags, mode) SPIFFS_open(&fs, name, flags, mode)

#define SEEK_SET SPIFFS_SEEK_SET
#define SEEK_CUR SPIFFS_SEEK_CUR
#define SEEK_END SPIFFS_SEEK_END
#define O_RDONLY SPIFFS_RDONLY
#define O_RDWR SPIFFS_RDWR
#define O_CREAT SPIFFS_CREAT

#endif /* STORAGE_INCLUDE_STORAGE_SPIFFS_POSIX_H_ */
