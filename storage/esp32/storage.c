
#include "storage/storage.h"
#include "debug/dbg.h"

#ifndef TEST_HOST
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#endif

#include <stdio.h>
#include <string.h>

#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

#define STRLCPY(dst,src,size) strlcpy((dst),(src),(size))
#define STRCPY(dst,src) strcpy((dst),(src))


#ifdef TEST_HOST
#define DB(x) x
#define DB2(x)
#define BASE_PATH "."
#else
#define DB(x) (TXTIO_IS_VERBOSE(vrbDebug) && (x),1)
#define DB2(x) DB(x)
#define BASE_PATH "/spiffs"
#endif


#define TAG "storage"


///////////// implement read/ write from storage.h ////////////////////////

bool
stor_fileWrite(const char *path, const void *src, size_t len) {
   char complete_path[strlen(BASE_PATH) + 1 + strlen(path) + 1];
   STRCPY(complete_path, BASE_PATH "/");
   strcat(complete_path, path);


   FILE* f = fopen(complete_path, "w");
   if (f == NULL) {
    // ESP_LOGE(TAG, "Failed to open file for writing");
     return false;
   }

   fwrite(src, len, 1, f);
   fclose(f);


  return true;

}

bool
stor_fileRead(const char *path, void *dst, size_t len) {
  char complete_path[strlen(BASE_PATH) + 1 + strlen(path) + 1];
  STRCPY(complete_path, BASE_PATH "/");
  strcat(complete_path, path);

  FILE* f = fopen(complete_path, "r");
  if (f == NULL) {
   // ESP_LOGE(TAG, "Failed to open file for reading");
    return false;
  }

  fread(dst, len, 1, f);
  fclose(f);

  return true;
}

bool
stor_fileDelete(const char *path) {
  char complete_path[strlen(BASE_PATH) + 1 + strlen(path) + 1];
  STRCPY(complete_path, BASE_PATH "/");
  strcat(complete_path, path);

  unlink(complete_path);

  return true;
}

#ifndef TEST_HOST
void stor_setup(void) {
  ESP_LOGI(TAG, "Initializing SPIFFS");

   esp_vfs_spiffs_conf_t conf = {
     .base_path = BASE_PATH,
     .partition_label = NULL,
     .max_files = 100,
     .format_if_mount_failed = true
   };

   // Use settings defined above to initialize and mount SPIFFS filesystem.
   // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
   esp_err_t ret = esp_vfs_spiffs_register(&conf);

   if (ret != ESP_OK) {
       if (ret == ESP_FAIL) {
           ESP_LOGE(TAG, "Failed to mount or format filesystem");
       } else if (ret == ESP_ERR_NOT_FOUND) {
           ESP_LOGE(TAG, "Failed to find SPIFFS partition");
       } else {
           ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
       }
       return;
   }

   size_t total = 0, used = 0;
   ret = esp_spiffs_info(NULL, &total, &used);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
   } else {
       ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
   }
}

#endif
