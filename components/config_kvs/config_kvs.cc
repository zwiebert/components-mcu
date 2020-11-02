/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#include "config_kvs/config.h"
#include "config_kvs.h"
#include "config_kvs/config_defaults.h"

#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "utils_misc/stof.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define D(x) 


unsigned nvsStr(kvshT handle, const char *key, void *dst, size_t dst_len, bool save) {
  if (save) {
    return kvs_rw_str(handle, key, static_cast<char*>(dst), dst_len, true);
  } else {
    return kvs_rw_str(handle, key, static_cast<char*>(dst), dst_len, false);
  }
}

unsigned nvsBlob(kvshT handle, const char *key, void *dst, size_t dst_len, bool save) {
    if (save) {
      return kvs_rw_blob(handle, key, dst, dst_len, true);
    } else {
      return kvs_rw_blob(handle, key, dst, dst_len, false);
    }
}


///////////////////////////////////////

const char* config_read_item_s(const char *key, char *d, unsigned d_size, const char *def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    if (kvs_rw_str(h, key, d, d_size, false))
      def = d;
    kvs_close(h);
  }
  return def;
}
const void* config_read_item_b(const char *key, void *d, unsigned d_size, const void *def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    if (kvs_rw_blob(h, key, d, d_size, false))
      def = d;
    kvs_close(h);
  }
  return def;
}

uint32_t config_read_item_u32(const char *key, uint32_t def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    def = kvs_get_u32(h, key, def, nullptr);
    kvs_close(h);
  }
  return def;
}

int8_t config_read_item_i8(const char *key, int8_t def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    def = kvs_get_i8(h, key, def, nullptr);
    kvs_close(h);
  }
  return def;
}
float config_read_item_f(const char *key, float def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvs_rw_blob(h, key, &def, sizeof def, false);
    kvs_close(h);
  }
  return def;
}



//////////////////////////////////////////////////////////


bool config_save_item_s(const char *key, const char *val) {
  kvshT h;
  bool result = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    result = strlen(val) == kvs_rw_str(h, key, (char*)val, 0, true);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}

bool config_save_item_b(const char *key, const void *val, unsigned size) {
  kvshT h;
  bool result = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    result = size == kvs_rw_blob(h, key, (void*)val, size, true);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}

bool config_save_item_u32(const char *key, const char *val, unsigned base) {
  u32 v = strtoul(val, 0, base);
  return config_save_item_n_u32(key, v);
}
bool config_save_item_i8(const char *key, const char *val) {
  i8 v = atoi(val);
  return config_save_item_n_i8(key, v);
}
bool config_save_item_f(const char *key, const char *val) {
  float v = stof(val);
  return config_save_item_n_f(key, v);
}
bool config_save_item_n_u32(const char *key, uint32_t val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvs_set_u32(h,key, val);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}
bool config_save_item_n_i8(const char *key, int8_t val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvs_set_i8(h, key, val);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}
bool config_save_item_n_f(const char *key, float val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvs_rw_blob(h, key, &val, sizeof val, true);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}

