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

const char * const configKvs_keys[] = {
  "C_VERBOSE",
#ifdef USE_WLAN
  "C_WIFI_SSID", "C_WIFI_PASSWD",
#endif
 #ifdef USE_MQTT
  "C_MQTT_URL", "C_MQTT_USER", "C_MQTT_PASSWD", "C_MQTT_CID", "C_MQTT_ENABLE", "C_MQTT_RTOPIC",
#endif
#ifdef USE_HTTP
  "C_HTTP_USER", "C_HTTP_PASSWD", "C_HTTP_ENABLE",
#endif
#ifdef USE_NTP
  "C_NTP_SERVER",
#endif
#ifdef USE_LAN
  "C_LAN_PHY", "C_LAN_PWR_GPIO",
#endif
};


const char *config_get_kvs_key(uint8_t cb) {
  return (((int)cb < (int)CB_size) ? configKvs_keys[(cb)] : config_keys[(cb-CB_size)]);
}

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

bool config_save_item_s(enum configItem item, const char *val) {
  kvshT h;
  bool result = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    result = strlen(val) == kvsWs(item, (char*)val);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}

bool config_save_item_b(enum configItem item, const void *val, unsigned size) {
  kvshT h;
  bool result = false;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    result = size == kvs_rw_blob(h, cfg_key(item), (void*)val, size, true);
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}

bool config_save_item_u32(enum configItem item, const char *val) {
  u32 v = strtoul(val, 0, 10);
  return config_save_item_n_u32(item, v);
}
bool config_save_item_i8(enum configItem item, const char *val) {
  i8 v = atoi(val);
  return config_save_item_n_i8(item, v);
}
bool config_save_item_f(enum configItem item, const char *val) {
  float v = stof(val);
  return config_save_item_n_f(item, v);
}
bool config_save_item_n_u32(enum configItem item, uint32_t val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvsW(u32, item, val);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}
bool config_save_item_n_i8(enum configItem item, int8_t val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvsW(i8, item, val);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}
bool config_save_item_n_f(enum configItem item, float val) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
    kvsWb(item, val);
    kvs_commit(h);
    kvs_close(h);
  }
  return true;
}


const char* config_read_item_s(enum configItem item, char *d, unsigned d_size, const char *def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    if (kvs_rw_str(h, cfg_key(item), d, d_size, false))
      def = d;
    kvs_close(h);
  }
  return def;
}
void* config_read_item_b(enum configItem item, void *d, unsigned d_size, void *def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    if (kvs_rw_blob(h, cfg_key(item), d, d_size, false))
      def = d;
    kvs_close(h);
  }
  return def;
}
uint32_t config_read_item_u32(enum configItem item, uint32_t def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(u32, item, def);
    kvs_close(h);
  }
  return def;
}
int8_t config_read_item_i8(enum configItem item, int8_t def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, item, def);
    kvs_close(h);
  }
  return def;
}
float config_read_item_f(enum configItem item, float def) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRb(item, def);
    kvs_close(h);
  }
  return def;
}

