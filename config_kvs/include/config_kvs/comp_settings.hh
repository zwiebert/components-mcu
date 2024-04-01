/**
 * \file config_kvs/comp_settings.hh
 * \brief Settings for shared components
 */

#pragma once
#include "settings_template.hh"
#include <app_config/options.hh>
#include <utils_misc/int_macros.h>
#include "config.h"
#include <assert.h>





enum configItem : int8_t {
  CBC_NONE = -1,
  CB_VERBOSE,
#ifdef CONFIG_APP_USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
#ifdef CONFIG_APP_USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_CLIENT_ID, CB_MQTT_ROOT_TOPIC, CB_MQTT_ENABLE,
#endif
#ifdef CONFIG_APP_USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef CONFIG_APP_USE_NTP
  CB_NTP_SERVER,
#endif
#ifdef CONFIG_APP_USE_LAN
  CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif
#ifdef CONFIG_STM32_USE_COMPONENT
  CB_STM32_INV_BOOTPIN,
#endif
//-----------
  CB_size
};

class Config_Item {
public:
  enum {
    CBC_NONE = -1, CB_VERBOSE,
#ifdef CONFIG_APP_USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
#ifdef CONFIG_APP_USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_CLIENT_ID, CB_MQTT_ROOT_TOPIC, CB_MQTT_ENABLE,
#endif
#ifdef CONFIG_APP_USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef CONFIG_APP_USE_NTP
  CB_NTP_SERVER,
#endif
#ifdef CONFIG_APP_USE_LAN
  CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif
#ifdef CONFIG_STM32_USE_COMPONENT
  CB_STM32_INV_BOOTPIN,
#endif
//-----------
    CB_size
  };
};
template<typename T>
constexpr Config_Item toConfigItem(T item) {
  return static_cast<Config_Item>(item);
}

extern const SettingsBase<configItem> &comp_sett;

#ifdef CONFIG_APP_USE_MQTT
constexpr uint32_t CBM_mqttClient = BIT(CB_MQTT_ENABLE) | BIT(CB_MQTT_PASSWD) | BIT(CB_MQTT_USER) | BIT(CB_MQTT_URL) | BIT(CB_MQTT_CLIENT_ID) | BIT(CB_MQTT_ROOT_TOPIC);
#endif
#ifdef CONFIG_APP_USE_HTTP
  constexpr uint32_t CBM_httpServer = BIT(CB_HTTP_ENABLE) | BIT(CB_HTTP_PASSWD) | BIT(CB_HTTP_USER);
#endif
#ifdef CONFIG_APP_USE_LAN
  constexpr uint32_t CMB_lan = BIT(CB_LAN_PHY) | BIT(CB_LAN_PWR_GPIO);
#endif
  constexpr uint32_t CBM_txtio = BIT(CB_VERBOSE);



bool config_item_modified(enum configItem item);


template<typename Kvs_Type, typename CfgItem>
constexpr Kvs_Type config_read_item(CfgItem item, Kvs_Type def) {
  switch (comp_sett.get_kvsType(item)) {
  case CBT_u32:
    return static_cast<Kvs_Type>(config_read_item_u32(comp_sett.get_kvsKey(item), (uint32_t) def));
  case CBT_i8:
    return static_cast<Kvs_Type>(config_read_item_i8(comp_sett.get_kvsKey(item), (int8_t) def));
  case CBT_f:
    return static_cast<Kvs_Type>(config_read_item_f(comp_sett.get_kvsKey(item), (float) def));
  default:
    assert(!"unhandled type");
  }
  return def;
}

template<typename Kvs_Type, typename CfgItem>
constexpr const Kvs_Type* config_read_item(CfgItem item, Kvs_Type *d, size_t d_size, const Kvs_Type *def) {
  switch (comp_sett.get_kvsType(item)) {
  case CBT_str:
    return static_cast<const Kvs_Type *>(config_read_item_s(comp_sett.get_kvsKey(item), d, d_size, (const char *)def));
  case CBT_blob:
    return static_cast<const Kvs_Type *>(config_read_item_b(comp_sett.get_kvsKey(item), d, d_size, (const void *)def));
  default:
    assert(!"unhandled type");
  }
  return def;
}
