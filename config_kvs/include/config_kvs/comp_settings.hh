/**
 * \file config_kvs/comp_settings.hh
 * \brief Settings for shared components
 */

#pragma once
#include "settings.hh"
#include <app_config/options.hh>
#include <uout/uo_config.h>
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
//-----------
  CB_size
};

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

extern SettingsBase<configItem> &comp_settings;

#if 0
const char* settings_get_kvsKey(configItem item);
KvsType settings_get_kvsType(configItem item);
otok settings_get_optKey(configItem item);
const char* settings_get_optKeyStr(configItem item);
SettingsBase<configItem>::soCfgFunT settings_get_soCfgFun(configItem item);


#else

constexpr const char* settings_get_kvsKey(configItem item) {
  return comp_settings.get_kvsKey(item);
}
constexpr KvsType settings_get_kvsType(configItem item) {
  return comp_settings.get_kvsType(item);
}
constexpr otok settings_get_optKey(configItem item) {
  return comp_settings.get_optKey(item);
}
constexpr const char* settings_get_optKeyStr(configItem item) {
  return comp_settings.get_optKeyStr(item);
}

constexpr SettingsBase<configItem>::soCfgFunT settings_get_soCfgFun(configItem item) {
  return comp_settings.get_soCfgFun(item);
}

#endif

bool config_item_modified(enum configItem item);
