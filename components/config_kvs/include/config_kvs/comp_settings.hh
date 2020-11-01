#pragma once
#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include "config.h"
#include <assert.h>

enum KvsType : u8 {
  CBT_none, CBT_i8, CBT_u8, CBT_i16, CBT_u16, CBT_i32, CBT_u32, CBT_i64, CBT_u64, CBT_str, CBT_f, CBT_blob, CBT_end
};

enum configItem {
  CB_VERBOSE,
#ifdef USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
#ifdef USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_CLIENT_ID, CB_MQTT_ROOT_TOPIC, CB_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  CB_NTP_SERVER,
#endif
#ifdef USE_LAN
  CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif
//-----------
  CB_size
};

class CompSettings {

private:
  constexpr void initField(const configItem ci, const char *const kvsKey, const otok optKey, const KvsType kvsType) {
    const unsigned idx = ci;
    cs_optKeys[idx] = optKey;
    cs_kvsKeys[idx] = kvsKey;
    cs_kvsTypes[idx] = kvsType;
  }

public:
  constexpr CompSettings() {
    initField(CB_VERBOSE, "C_VERBOSE", otok::k_verbose, CBT_i8);
#ifdef USE_WLAN
    initField(CB_WIFI_SSID, "C_WIFI_SSID", otok::k_wlan_ssid, CBT_str);
    initField(CB_WIFI_PASSWD, "C_WIFI_PASSWD", otok::k_wlan_password, CBT_str);
#endif
#ifdef USE_MQTT
    initField(CB_MQTT_URL, "C_MQTT_URL", otok::k_mqtt_url, CBT_str);
    initField(CB_MQTT_USER, "C_MQTT_USER", otok::k_mqtt_user, CBT_str);
    initField(CB_MQTT_PASSWD, "C_MQTT_PASSWD", otok::k_mqtt_password, CBT_str);
    initField(CB_MQTT_CLIENT_ID, "C_MQTT_CID", otok::k_mqtt_client_id, CBT_str);
    initField(CB_MQTT_ROOT_TOPIC, "C_MQTT_RTOPIC", otok::k_mqtt_root_topic, CBT_str);
    initField(CB_MQTT_ENABLE, "C_MQTT_ENABLE", otok::k_mqtt_enable, CBT_i8);
#endif
#ifdef USE_HTTP
    initField(CB_HTTP_USER, "C_HTTP_USER", otok::k_http_user, CBT_str);
    initField(CB_HTTP_PASSWD, "C_HTTP_PASSWD", otok::k_http_password, CBT_str);
    initField(CB_HTTP_ENABLE, "C_HTTP_ENABLE", otok::k_http_enable, CBT_i8);
#endif
#ifdef USE_NTP
    initField(CB_NTP_SERVER, "C_NTP_SERVER", otok::k_ntp_server, CBT_str);
#endif
#ifdef USE_LAN
    initField(CB_LAN_PHY, "C_LAN_PHY", otok::k_lan_phy, CBT_i8);
    initField(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", otok::k_lan_pwr_gpio, CBT_i8);
    // initField(, "", otok::k_);
#endif
  }

public:

  constexpr const char* get_kvsKey(configItem idx) const {
    return cs_kvsKeys[idx];
  }
  constexpr KvsType get_kvsType(configItem idx) const {
    return cs_kvsTypes[idx];
  }

  constexpr otok get_optKey(configItem idx) const {
    return cs_optKeys[idx];
  }
  constexpr const char* get_optKeyStr(configItem idx) const {
    return otok_strings[static_cast<otokBaseT>(cs_optKeys[idx])];
  }

  struct Item {
    const char *kvsKey;
    otok optKey;
  };

public:
  const char *cs_kvsKeys[CB_size] { };
  otok cs_optKeys[CB_size] { };
  KvsType cs_kvsTypes[CB_size] { };
};

constexpr CompSettings comp_settings;

template<typename Kvs_Type>
Kvs_Type config_read_item(configItem item, Kvs_Type def) {
  switch (comp_settings.get_kvsType(item)) {
  case CBT_u32:
    return config_read_item_i8(comp_settings.get_kvsKey(item), (u32) def);
  case CBT_i8:
    return config_read_item_i8(comp_settings.get_kvsKey(item), (i8) def);
  case CBT_f:
    return config_read_item_f(comp_settings.get_kvsKey(item), (float) def);
  default:
    assert(!"unhandled type");
  }
  return def;
}

template<typename Kvs_Type>
const Kvs_Type* config_read_item(configItem item, Kvs_Type *d, size_t d_size, const Kvs_Type *def) {
  switch (comp_settings.get_kvsType(item)) {
  case CBT_str:
    return config_read_item_s(comp_settings.get_kvsKey(item), d, d_size, (const char*) def);
  default:
    assert(!"unhandled type");
  }
  return def;
}

inline const char* cfg_key(configItem item) {
  return comp_settings.get_kvsKey(item);
}
inline const char* cfg_optStr(configItem item) {
  return comp_settings.get_optKeyStr(item);
}
