/*
 * kvs_implementation.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */





#include "utils_misc/int_types.h"
#include "key_value_store/kvs_wrapper.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define CFG_PARTNAME "nvs"

struct kvs_handle {
  nvs_handle handle;
};

#define VP2H(vp)((vp)->handle)

kvshT kvs_open(const char *name, unsigned flags) {
  static struct kvs_handle kvs_handle;
  int nvs_flag = (flags & kvs_WRITE) ? NVS_READWRITE : NVS_READONLY;

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

size_t kvs_rw_str(kvshT handle, const char *key, char *src_or_dst, unsigned length, bool write) {
  if (write) {
    if (nvs_set_str(VP2H(handle), key, src_or_dst) == ESP_OK)
      return 1;
  } else {
    size_t dst_len = length;
    if (nvs_get_str(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
      return (int)dst_len;
    }
  }
  return 0;
}

size_t kvs_rw_blob(kvshT handle, const char *key, void *src_or_dst, unsigned length, bool write) {
  if (write) {
    if (nvs_set_blob(VP2H(handle), key, src_or_dst, length) == ESP_OK)
      return 1;
  } else {
    size_t dst_len = length;
    if (nvs_get_blob(VP2H(handle), key, src_or_dst, &dst_len) == ESP_OK) {
      return (int)dst_len;
    }
  }
  return 0;
}

#if 0
#define SET_DT_FUN(DT) \
bool kvs_set_##DT (kvshT handle, const char *key, DT val) { \
  return nvs_set_##DT(VP2H(handle), key, val) == ESP_OK; \
} \

#define GET_DT_FUN(DT) \
DT kvs_get_##DT(kvshT handle, const char *key, DT default_val, bool *res_success) { \
  DT temp; \
  if (ESP_OK == nvs_get_##DT(VP2H(handle), key, &temp)) { \
    if (res_success)*res_success = true; \
    return temp; \
  } \
  if (res_success) *res_success = false; \
  return default_val; \
} \

#define SET_GET_DT_FUN(dt) SET_DT_FUN(dt) GET_DT_FUN(dt);


SET_GET_DT_FUN(int8_t);
SET_GET_DT_FUN(uint8_t);
SET_GET_DT_FUN(int16_t);
SET_GET_DT_FUN(uint16_t);
SET_GET_DT_FUN(int32_t);
SET_GET_DT_FUN(uint32_t);
SET_GET_DT_FUN(int64_t);
SET_GET_DT_FUN(u64);
#else

bool kvs_set_i8(kvshT handle, const char *key, int8_t val) {
  return nvs_set_i8(VP2H(handle), key, val) == ESP_OK;
}
int8_t kvs_get_i8(kvshT handle, const char *key, int8_t default_val, bool *res_success) {
  int8_t temp;
  if (ESP_OK == nvs_get_i8(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_u8(kvshT handle, const char *key, uint8_t val) {
  return nvs_set_u8(VP2H(handle), key, val) == ESP_OK;
}
uint8_t kvs_get_u8(kvshT handle, const char *key, uint8_t default_val, bool *res_success) {
  uint8_t temp;
  if (ESP_OK == nvs_get_u8(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_i16(kvshT handle, const char *key, int16_t val) {
  return nvs_set_i16(VP2H(handle), key, val) == ESP_OK;
}
int16_t kvs_get_i16(kvshT handle, const char *key, int16_t default_val, bool *res_success) {
  int16_t temp;
  if (ESP_OK == nvs_get_i16(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_u16(kvshT handle, const char *key, uint16_t val) {
  return nvs_set_u16(VP2H(handle), key, val) == ESP_OK;
}
uint16_t kvs_get_u16(kvshT handle, const char *key, uint16_t default_val, bool *res_success) {
  uint16_t temp;
  if (ESP_OK == nvs_get_u16(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_i32(kvshT handle, const char *key, int32_t val) {
  return nvs_set_i32(VP2H(handle), key, val) == ESP_OK;
}
int32_t kvs_get_i32(kvshT handle, const char *key, int32_t default_val, bool *res_success) {
  int32_t temp;
  if (ESP_OK == nvs_get_i32(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_u32(kvshT handle, const char *key, uint32_t val) {
  return nvs_set_u32(VP2H(handle), key, val) == ESP_OK;
}
uint32_t kvs_get_u32(kvshT handle, const char *key, uint32_t default_val, bool *res_success) {
  uint32_t temp;
  if (ESP_OK == nvs_get_u32(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_i64(kvshT handle, const char *key, int64_t val) {
  return nvs_set_i64(VP2H(handle), key, val) == ESP_OK;
}
int64_t kvs_get_i64(kvshT handle, const char *key, int64_t default_val, bool *res_success) {
  int64_t temp;
  if (ESP_OK == nvs_get_i64(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;
bool kvs_set_u64(kvshT handle, const char *key, u64 val) {
  return nvs_set_u64(VP2H(handle), key, val) == ESP_OK;
}
u64 kvs_get_u64(kvshT handle, const char *key, u64 default_val, bool *res_success) {
  u64 temp;
  if (ESP_OK == nvs_get_u64(VP2H(handle), key, &temp)) {
    if (res_success)
      *res_success = true;
    return temp;
  }
  if (res_success)
    *res_success = false;
  return default_val;
}
;
;

#endif

int kvs_foreach(const char *name_space, kvs_type_t type, const char *key_match, kvs_foreach_cbT cb, void *args) {
  nvs_iterator_t it = NULL;
  esp_err_t res = nvs_entry_find(CFG_PARTNAME, name_space, type, &it);

  int count = 0;
  while (res == ESP_OK) {
    nvs_entry_info_t info;
    nvs_entry_info(it, &info);
    res = nvs_entry_next(&it);

    if (key_match) {
      unsigned len = strlen(key_match);
      if (strlen(info.key) < len)
        continue;
      if (0 != strncmp(info.key, key_match, len))
        continue;
    }

    if (cb) {
      switch (cb(info.key, info.type, args)) {
      case kvsCb_match:
        ++count;
        break;
      case kvsCb_done:
        ++count;
        return count;
      case kvsCb_noMatch:
        break;
      }
    } else {
      ++count;
    }
  }
  return count;
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
