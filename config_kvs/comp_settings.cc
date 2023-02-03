/**
 * \file config_kvs/comp_settings.hh
 * \brief Settings for shared components
 */

#include "config_kvs/settings.hh"
#include "config_kvs/comp_settings.hh"
#include <config_kvs/settings_template.hh>
#include <app_config/options.hh>
#include <uout/uo_config.h>
#include <utils_misc/int_macros.h>
#include "config_kvs/config.h"
#include <assert.h>

class CompSettings final : public Settings<configItem, CB_size> {
public:
  using Base = Settings<configItem, CB_size>;
public:
  constexpr CompSettings() {
    initField(CB_VERBOSE, "C_VERBOSE", otok::k_verbose, CBT_i8, soCfg_VERBOSE, STF_direct);
#ifdef CONFIG_APP_USE_WLAN
    initField(CB_WIFI_SSID, "C_WIFI_SSID", otok::k_wlan_ssid, CBT_str, soCfg_WLAN_SSID, STF_direct);
    initField(CB_WIFI_PASSWD, "C_WIFI_PASSWD", otok::k_wlan_password, CBT_str, soCfg_WLAN_PASSWORD, STF_direct);
#endif
#ifdef CONFIG_APP_USE_MQTT
    initField(CB_MQTT_ENABLE, "C_MQTT_ENABLE", otok::k_mqtt_enable, CBT_i8, soCfg_MQTT_ENABLE, STF_direct_bool);
    initField(CB_MQTT_URL, "C_MQTT_URL", otok::k_mqtt_url, CBT_str, soCfg_MQTT_URL, STF_direct);
    initField(CB_MQTT_USER, "C_MQTT_USER", otok::k_mqtt_user, CBT_str, soCfg_MQTT_USER, STF_direct);
    initField(CB_MQTT_PASSWD, "C_MQTT_PASSWD", otok::k_mqtt_password, CBT_str, soCfg_MQTT_PASSWORD, STF_direct);
    initField(CB_MQTT_CLIENT_ID, "C_MQTT_CID", otok::k_mqtt_client_id, CBT_str, soCfg_MQTT_CLIENT_ID, STF_direct);
    initField(CB_MQTT_ROOT_TOPIC, "C_MQTT_RTOPIC", otok::k_mqtt_root_topic, CBT_str, soCfg_MQTT_ROOT_TOPIC, STF_direct);
#endif
#ifdef CONFIG_APP_USE_HTTP
    initField(CB_HTTP_ENABLE, "C_HTTP_ENABLE", otok::k_http_enable, CBT_i8, soCfg_HTTP_ENABLE, STF_direct_bool);
    initField(CB_HTTP_USER, "C_HTTP_USER", otok::k_http_user, CBT_str, soCfg_HTTP_USER, STF_direct);
    initField(CB_HTTP_PASSWD, "C_HTTP_PASSWD", otok::k_http_password, CBT_str, soCfg_HTTP_PASSWORD, STF_direct);
#endif
#ifdef CONFIG_APP_USE_NTP
    initField(CB_NTP_SERVER, "C_NTP_SERVER", otok::k_ntp_server, CBT_str, soCfg_NTP_SERVER, STF_direct);
#endif
#ifdef CONFIG_APP_USE_LAN
    initField(CB_LAN_PHY, "C_LAN_PHY", otok::k_lan_phy, CBT_i8, soCfg_LAN_PHY);
    initField(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", otok::k_lan_pwr_gpio, CBT_i8, soCfg_LAN_PWR_GPIO);
    // initField(, "", otok::k_);
#endif
  }
};

static constexpr CompSettings comp_settings_obj;
const SettingsBase<configItem> &comp_sett = comp_settings_obj;


