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
#ifdef CONFIG_STM32_USE_COMPONENT
  CB_STM32_INV_BOOTPIN,
#endif
//-----------
  CB_size
};

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




/// overloaded function for template usage
constexpr auto settings_get_kvsKey(configItem item) {
  return comp_sett.get_kvsKey(item);
}
/// overloaded function for template usage
constexpr auto settings_get_kvsType(configItem item) {
  return comp_sett.get_kvsType(item);
}
/// overloaded function for template usage
constexpr auto settings_get_optKey(configItem item) {
  return comp_sett.get_optKey(item);
}
/// overloaded function for template usage
constexpr auto settings_get_optKeyStr(configItem item) {
  return comp_sett.get_optKeyStr(item);
}

bool config_item_modified(enum configItem item);
