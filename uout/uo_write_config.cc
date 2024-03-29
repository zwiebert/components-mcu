#include <uout/so_target_desc.hh>
#include <config_kvs/comp_settings.hh>

#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef UOUT_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "uout.write_config"


///////////////////////////////////////////////////////////

#ifdef CONFIG_APP_USE_LAN
#include "net/ethernet_setup.hh"
void soCfg_LAN_PHY(class UoutWriter &td) {
  cfg_lan c;
  td.so().print(settings_get_optKeyStr(CB_LAN_PHY), cfg_args_lanPhy[config_read_ethernet(&c)->phy]);
}
void soCfg_LAN_PWR_GPIO(class UoutWriter &td) {
  cfg_lan c;
  td.so().print(settings_get_optKeyStr(CB_LAN_PWR_GPIO), config_read_ethernet(&c)->pwr_gpio);
}
#endif
#ifdef CONFIG_APP_USE_WLAN
#include "net/wifi_station_setup.hh"
void soCfg_WLAN_SSID(class UoutWriter &td) {
  cfg_wlan c;
  td.so().print(settings_get_optKeyStr(CB_WIFI_SSID), config_read_wifiStation(&c)->SSID);
}
void soCfg_WLAN_PASSWORD(class UoutWriter &td) {
  cfg_wlan c;
  td.so().print(settings_get_optKeyStr(CB_WIFI_PASSWD), *config_read_wifiStation(&c)->password ? "*" : "");
}
#endif
#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
void soCfg_NTP_SERVER(class UoutWriter &td) {
  cfg_ntp c;
  td.so().print(settings_get_optKeyStr(CB_NTP_SERVER), config_read_ntpClient(&c)->server);
}
#endif


#ifdef CONFIG_APP_USE_MQTT
#include "net_mqtt/mqtt.hh"

void soCfg_MQTT(class UoutWriter &td, bool backup) {
  cfg_mqtt c;
  config_read_mqttClient(&c);

  td.so().print(settings_get_optKeyStr(CB_MQTT_URL), c.url);
  td.so().print(settings_get_optKeyStr(CB_MQTT_USER), c.user);
  if (!backup) td.so().print(settings_get_optKeyStr(CB_MQTT_PASSWD), *c.password ? "*" : "");
  td.so().print(settings_get_optKeyStr(CB_MQTT_CLIENT_ID), c.client_id);
  td.so().print(settings_get_optKeyStr(CB_MQTT_ROOT_TOPIC), c.root_topic);
  td.so().print(settings_get_optKeyStr(CB_MQTT_ENABLE), c.enable);
}

void soCfg_MQTT_ENABLE(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_ENABLE), config_read_mqttClient(&c)->enable);
}
void soCfg_MQTT_URL(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_URL), config_read_mqttClient(&c)->url);
}
void soCfg_MQTT_USER(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_USER), config_read_mqttClient(&c)->user);
}
void soCfg_MQTT_PASSWORD(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_PASSWD), *config_read_mqttClient(&c)->password ? "*" : "");
}
void soCfg_MQTT_CLIENT_ID(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_CLIENT_ID), config_read_mqttClient(&c)->client_id);
}
void soCfg_MQTT_ROOT_TOPIC(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(settings_get_optKeyStr(CB_MQTT_ROOT_TOPIC), config_read_mqttClient(&c)->root_topic);
}
#endif


#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
void soCfg_HTTP(class UoutWriter &td) {
  cfg_http c;
  config_read_httpServer(&c);

  td.so().print(settings_get_optKeyStr(CB_HTTP_USER), c.user);
  td.so().print(settings_get_optKeyStr(CB_HTTP_PASSWD), *c.password ? "*" : "");
  td.so().print(settings_get_optKeyStr(CB_HTTP_ENABLE), c.enable);
}

void soCfg_HTTP_ENABLE(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_ENABLE), config_read_httpServer(&c)->enable);
}
void soCfg_HTTP_USER(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_USER), config_read_httpServer(&c)->user);
}
void soCfg_HTTP_PASSWORD(class UoutWriter &td) {
  cfg_http c;
  td.so().print(settings_get_optKeyStr(CB_HTTP_PASSWD), *config_read_httpServer(&c)->password ? "*" : "");
}
#endif

void soCfg_VERBOSE(class UoutWriter &td) {
  td.so().print(settings_get_optKeyStr(CB_VERBOSE), config_read_verbose());
}
#ifdef CONFIG_STM32_USE_COMPONENT
void soCfg_STM32_BOOTGPIO_INV(class UoutWriter &td) {
  td.so().print(settings_get_optKeyStr(CB_STM32_INV_BOOTPIN), config_read_stm32_inv_bootpin());
}
#endif
