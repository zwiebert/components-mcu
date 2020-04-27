/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "config_kvs/config_defaults.h"
#include "txtio/inout.h"
#include "net/ethernet.h"
#include "stdbool.h"


enum configItem {
 CB_VERBOSE,
#ifdef USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
#ifdef USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_CLIENT_ID, CB_MQTT_ENABLE,
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

bool config_save_item_s(enum configItem item, const char *val);
bool config_save_item_b(enum configItem item, const void *val, unsigned size);
bool config_save_item_u32(enum configItem item, const char *val);
bool config_save_item_i8(enum configItem item, const char *val);
bool config_save_item_f(enum configItem item, const char *val);
bool config_save_item_n_u32(enum configItem item, uint32_t val);
bool config_save_item_n_i8(enum configItem item, int8_t val);
bool config_save_item_n_f(enum configItem item, float val);

const char *config_read_item_s(enum configItem item, char *d, unsigned d_size, const char *def);
uint32_t config_read_item_u32(enum configItem item, uint32_t def);
int8_t config_read_item_i8(enum configItem item,  int8_t def);
float config_read_item_f(enum configItem item, float def);


void config_setup_cliTcpServer();
void config_setup_txtio();
void config_setup_mqttClient();
void config_setup_httpServer();
void config_setup_wifiStation();
void config_setup_ntpClient();
void config_setup_ethernet();

