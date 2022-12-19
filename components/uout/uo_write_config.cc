#include "app_config/proj_app_cfg.h"
#include <uout/so_target_desc.hh>
#include <config_kvs/comp_settings.hh>

#include "net/ipnet.h"
#include "net_mqtt/mqtt.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define D(x)


///////////////////////////////////////////////////////////

#ifdef CONFIG_APP_USE_LAN
void soCfg_LAN_PHY(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_LAN_PHY), cfg_args_lanPhy[config_read_lan_phy()]);
}
void soCfg_LAN_PWR_GPIO(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_LAN_PWR_GPIO), config_read_lan_pwr_gpio());
}
#endif
#ifdef CONFIG_APP_USE_WLAN
void soCfg_WLAN_SSID(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_WIFI_SSID), config_read_wifi_ssid(buf, sizeof buf));
}
void soCfg_WLAN_PASSWORD(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_WIFI_PASSWD), config_read_wifi_passwd(buf, sizeof buf) ? "*" : "");
}
#endif
#ifdef CONFIG_APP_USE_NTP
void soCfg_NTP_SERVER(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_NTP_SERVER), config_read_ntp_server(buf, sizeof buf));
}
#endif
#ifdef CONFIG_APP_USE_MQTT
void soCfg_MQTT_ENABLE(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_MQTT_ENABLE), config_read_mqtt_enable());
}
void soCfg_MQTT_URL(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_MQTT_URL), config_read_mqtt_url(buf, sizeof buf));
}
void soCfg_MQTT_USER(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_MQTT_USER), config_read_mqtt_user(buf, sizeof buf));
}
void soCfg_MQTT_PASSWORD(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_MQTT_PASSWD), *config_read_mqtt_passwd(buf, sizeof buf) ? "*" : "");
}
void soCfg_MQTT_CLIENT_ID(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_MQTT_CLIENT_ID), config_read_mqtt_client_id(buf, sizeof buf));
}
void soCfg_MQTT_ROOT_TOPIC(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_MQTT_ROOT_TOPIC), config_read_mqtt_root_topic(buf, sizeof buf));
}
#endif

#ifdef CONFIG_APP_USE_HTTP
void soCfg_HTTP_ENABLE(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_HTTP_ENABLE), config_read_http_enable());
}
void soCfg_HTTP_USER(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_HTTP_USER), config_read_http_user(buf, sizeof buf));
}
void soCfg_HTTP_PASSWORD(const struct TargetDesc &td) {
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_HTTP_PASSWD), *config_read_http_passwd(buf, sizeof buf) ? "*" : "");
}
#endif

void soCfg_VERBOSE(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_VERBOSE), config_read_verbose());
}
