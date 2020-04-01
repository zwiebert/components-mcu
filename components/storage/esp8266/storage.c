/*
 * storage.c
 *
 *  Created on: 13.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifndef NO_SPIFFS



#include "config/config.h"
#include "storage/storage.h"
#include "debug/debug.h"
#include "txtio/inout.h"

#ifdef TEST_HOST
#define DB(x)
#define DB2(x)
#else
#define DB(x) ((C.app_verboseOutput >= vrbDebug) && (x),1)
#define DB2(x) DB(x)
#include "spiffs.h"
#include "storage/spiffs_fs.h"
#endif

#include "storage/spiffs_posix.h"

void spiffs_print_errno() {
  io_printf("errno: IMPLEMENT_ME");
}
///////////// implement read/ write from storage.h ////////////////////////

bool 
stor_fileWrite(const char *path, const void *src, size_t len) {
  int file;
   i32 nmb_written;


   if (!path)
     return false;

   if ((file = open(path, O_TRUNC | O_CREAT | O_WRONLY, 0666)) < 0) {
       spiffs_print_errno();
       return false; // error
   }

   nmb_written = write(file, (void*)src, len);
   close(file);
   if (nmb_written < 0) {
     DB((io_puts("spiffs:errno: "), io_putl(errno(fs_A), 10),  io_puts("\n")));
   }

   DB((io_putd(nmb_written), io_puts("<-written-\n")));
   return true;

  DB2(printf("file saved: fid=%s\n", path));

  return true;

}

bool 
stor_fileRead(const char *path, void *dst, size_t len) {
  int file;
    i32 nmb_read = 0;

    if (!path)
      return false;

    if ((file = open(path, O_RDONLY, 0)) < 0) {
        return false;
    }

    nmb_read = read(file, dst, len);
    close(file);

    if (nmb_read < 0) {
      DB((io_puts("spiffs:errno: "), io_putl(errno(fs_A), 10),  io_puts("\n")));
      return false;
    }

   DB(printf("file read: file=%s\n", path));

   return true;
}

bool 
stor_fileDelete(const char *path) {
  return (unlink(path) >= 0);
}

void stor_setup(void) {
#if !(defined NO_SPIFFS || defined TEST_HOST)
  setup_spiffs();
#endif
}




#endif /* ENABLE_SPIFFS */
