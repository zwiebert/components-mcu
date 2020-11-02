/**
 * \file config_kvs/comp_settings.hh
 * \brief Settings for shared components
 */

#pragma once
#include "settings.hh"

#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include <uout/uo_config.h>
#include "config.h"
#include <assert.h>

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

class CompSettings : public Settings<configItem, CB_size> {
public:
  using Base = Settings<configItem, CB_size>;
public:
  constexpr CompSettings() {
    initField(CB_VERBOSE, "C_VERBOSE", otok::k_verbose, CBT_i8, soCfg_VERBOSE);
#ifdef USE_WLAN
    initField(CB_WIFI_SSID, "C_WIFI_SSID", otok::k_wlan_ssid, CBT_str, soCfg_WLAN_SSID);
    initField(CB_WIFI_PASSWD, "C_WIFI_PASSWD", otok::k_wlan_password, CBT_str, soCfg_WLAN_PASSWORD);
#endif
#ifdef USE_MQTT
    initField(CB_MQTT_ENABLE, "C_MQTT_ENABLE", otok::k_mqtt_enable, CBT_i8, soCfg_MQTT_ENABLE);
    initField(CB_MQTT_URL, "C_MQTT_URL", otok::k_mqtt_url, CBT_str, soCfg_MQTT_URL);
    initField(CB_MQTT_USER, "C_MQTT_USER", otok::k_mqtt_user, CBT_str, soCfg_MQTT_USER);
    initField(CB_MQTT_PASSWD, "C_MQTT_PASSWD", otok::k_mqtt_password, CBT_str, soCfg_MQTT_PASSWORD);
    initField(CB_MQTT_CLIENT_ID, "C_MQTT_CID", otok::k_mqtt_client_id, CBT_str, soCfg_MQTT_CLIENT_ID);
    initField(CB_MQTT_ROOT_TOPIC, "C_MQTT_RTOPIC", otok::k_mqtt_root_topic, CBT_str, soCfg_MQTT_ROOT_TOPIC);
#endif
#ifdef USE_HTTP
    initField(CB_HTTP_ENABLE, "C_HTTP_ENABLE", otok::k_http_enable, CBT_i8, soCfg_HTTP_ENABLE);
    initField(CB_HTTP_USER, "C_HTTP_USER", otok::k_http_user, CBT_str, soCfg_HTTP_USER);
    initField(CB_HTTP_PASSWD, "C_HTTP_PASSWD", otok::k_http_password, CBT_str, soCfg_HTTP_PASSWORD);
#endif
#ifdef USE_NTP
    initField(CB_NTP_SERVER, "C_NTP_SERVER", otok::k_ntp_server, CBT_str, soCfg_NTP_SERVER);
#endif
#ifdef USE_LAN
    initField(CB_LAN_PHY, "C_LAN_PHY", otok::k_lan_phy, CBT_i8, soCfg_LAN_PHY);
    initField(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", otok::k_lan_pwr_gpio, CBT_i8, soCfg_LAN_PWR_GPIO);
    // initField(, "", otok::k_);
#endif
  }
};

constexpr CompSettings comp_settings;

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

constexpr CompSettings::Base::soCfgFunT settings_get_soCfgFun(configItem item) {
  return comp_settings.get_soCfgFun(item);
}
bool config_item_modified(enum configItem item);
