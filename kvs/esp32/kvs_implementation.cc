/*
 * kvs_implementation.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */





#include "utils_misc/int_types.h"
#include "utils_misc/cstring_utils.h"
#include "kvs/kvs_wrapper.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define logtag "kvs"
struct kvs_handle {
  nvs_handle handle;
};

#define VP2H(vp)((vp)->handle)

kvshT kvs_open(const char *name, unsigned flags) {
  static struct kvs_handle kvs_handle;
  nvs_open_mode_t nvs_flag = (nvs_open_mode_t) (flags & kvs_WRITE) ? NVS_READWRITE : NVS_READONLY;

  esp_err_t err  = nvs_open(name, nvs_flag, &kvs_handle.handle);
  if (err == ESP_OK)  {
    return &kvs_handle;
  }

  return 0;
}

bool kvs_commit(kvshT handle) {
  return nvs_commit(VP2H(handle)) == ESP_OK;
}

void kvs_close(kvshT handle) {
  nvs_close(VP2H(handle));
  handle = 0;
}

bool kvs_erase_key(kvshT handle, const char *key) {
  return ESP_OK == nvs_erase_key(VP2H(handle), key);
}

static ssize_t kvs_read_str(kvshT handle, const char *key, char *src_or_dst, unsigned length) {
  size_t dst_len = length;
  if (nvs_get_str(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
    return (int) dst_len;
  }
  return -1;
}

bool kvs_write_str(kvshT handle, const char *key, const char *src_or_dst) {
  return (nvs_set_str(VP2H(handle), key, src_or_dst) == ESP_OK);
}

bool kvs_write_blob(kvshT handle, const char *key, const void *src_or_dst, unsigned length) {
  if (auto err = nvs_set_blob(VP2H(handle), key, src_or_dst, length); err != ESP_OK) {
     ESP_LOGE(logtag, "%s: %s", __func__, esp_err_to_name(err));
     return false;
  }
  return true;
}

static ssize_t kvs_read_blob(kvshT handle, const char *key, void *src_or_dst, unsigned length) {
  size_t dst_len = length;
  if (nvs_get_blob(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
    return dst_len;
  }
  return -1;
}

bool kvs_set_blob(kvshT handle, const char *key, const void *val, size_t val_size) {
  return kvs_write_blob(handle, key, val, val_size);
}

bool kvs_get_blob(kvshT handle, const char *key, void *dst, size_t dst_size) {
  auto n = kvs_read_blob(handle, key, dst, dst_size);
  return 0 <= n && dst_size == n;
}



bool kvs_set_str(kvshT handle, const char *key, const char *val) {
  return kvs_write_str(handle, key, val);
}

bool kvs_get_str(kvshT handle, const char *key, char *dst, size_t dst_size) {
  auto n = kvs_read_str(handle, key, dst, dst_size);
  return 0 <= n && n <= dst_size;
}

// macros to generate set/get for integer types
#define SET_DT_FUN(DT,ST) \
bool kvs_set##ST (kvshT handle, const char *key, DT val) { \
  return nvs_set##ST(VP2H(handle), key, val) == ESP_OK; \
} \

#define GET_DT_FUN(DT,ST) \
DT kvs_get##ST(kvshT handle, const char *key, DT default_val, bool *res_success) { \
  DT temp; \
  if (ESP_OK == nvs_get##ST(VP2H(handle), key, &temp)) { \
    if (res_success)*res_success = true; \
    return temp; \
  } \
  if (res_success) *res_success = false; \
  return default_val; \
} \

#define SET_GET_DT_FUN(dt,st) SET_DT_FUN(dt,st) GET_DT_FUN(dt,st);

SET_GET_DT_FUN(int8_t, _i8);
SET_GET_DT_FUN(uint8_t,_u8);
SET_GET_DT_FUN(int16_t, _i16);
SET_GET_DT_FUN(uint16_t, _u16);
SET_GET_DT_FUN(int32_t, _i32);
SET_GET_DT_FUN(uint32_t, _u32);
SET_GET_DT_FUN(int64_t, _i64);
SET_GET_DT_FUN(uint64_t, _u64);

int kvs_foreach(const char *name_space, kvs_type_t type, const char *key_match, kvs_foreach_cbT cb, void *cb_args) {
  return kvs_foreach(name_space, type, csu_startsWith, key_match, cb, cb_args);
}



void kvs_setup(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK( err );
}
