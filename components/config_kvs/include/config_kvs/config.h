#ifdef __cplusplus
  extern "C" {
#endif
/*
 * config.h
 *
 * Created: 24.08.2017 20:56:00
 *  Author: bertw
 */

#pragma once

#include "app/config/proj_app_cfg.h"
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


extern const char * const config_keys[]; //XXX: defined in app
extern const char * const configKvs_keys[];

const char *config_get_kvs_key(uint8_t cb);

bool config_save_item_s(enum configItem item, const char *val);
bool config_save_item_b(enum configItem item, const void *val, unsigned size);
bool config_save_item_u32(enum configItem item, const char *val);
bool config_save_item_i8(enum configItem item, const char *val);
bool config_save_item_f(enum configItem item, const char *val);
bool config_save_item_n_u32(enum configItem item, uint32_t val);
bool config_save_item_n_i8(enum configItem item, int8_t val);
bool config_save_item_n_f(enum configItem item, float val);

const char* config_read_item_s(enum configItem item, char *d, unsigned d_size, const char *def);
void* config_read_item_b(enum configItem item, void *d, unsigned d_size, void *def);
uint32_t config_read_item_u32(enum configItem item, uint32_t def);
int8_t config_read_item_i8(enum configItem item, int8_t def);
float config_read_item_f(enum configItem item, float def);

void config_setup_mqttClient();
struct cfg_mqtt* config_read_mqttClient(struct cfg_mqtt *c);
const char* config_read_mqtt_url(char *d, unsigned d_size);
const char* config_read_mqtt_user(char *d, unsigned d_size);
const char* config_read_mqtt_passwd(char *d, unsigned d_size);
const char* config_read_mqtt_client_id(char *d, unsigned d_size);
bool config_read_mqtt_enable();

void config_setup_httpServer();
struct cfg_http* config_read_httpServer(struct cfg_http *c);
const char* config_read_http_user(char *d, unsigned d_size);
const char* config_read_http_passwd(char *d, unsigned d_size);
bool config_read_http_enable();

void config_setup_wifiStation();
struct cfg_wlan* config_read_wifiStation(struct cfg_wlan *c);
const char* config_read_wifi_ssid(char *d, unsigned d_size);
const char* config_read_wifi_passwd(char *d, unsigned d_size);
bool config_read_wifi_enable();

void config_setup_ntpClient();
struct cfg_ntp* config_read_ntpClient(struct cfg_ntp *c);
const char* config_read_ntp_server(char *d, unsigned d_size);

void config_setup_ethernet();
struct cfg_lan* config_read_ethernet(struct cfg_lan *c);
int8_t config_read_lan_phy();
int8_t config_read_lan_pwr_gpio();

void config_setup_txtio();
struct cfg_txtio* config_read_txtio(struct cfg_txtio *c);
enum verbosity config_read_verbose();

void config_setup_cliTcpServer();

#ifdef __cplusplus
  }
#endif
