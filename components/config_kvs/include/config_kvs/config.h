/**
 * \file config_kvs/config.h
 */
#pragma once
#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>
#include "stdbool.h"

bool config_save_item_s(const char *key, const char *val);
bool config_save_item_b(const char *key, const void *val, unsigned size);
bool config_save_item_u32(const char *key, const char *val, unsigned base = 10);
bool config_save_item_i8(const char *key, const char *val);
bool config_save_item_f(const char *key, const char *val);
bool config_save_item_n_u32(const char *key, uint32_t val);
bool config_save_item_n_i8(const char *key, int8_t val);
bool config_save_item_n_f(const char *key, float val);

const void* config_read_item_b(const char *key, void *d, unsigned d_size, const void *def);
const char* config_read_item_s(const char *key, char *d, unsigned d_size, const char *def);
uint32_t config_read_item_u32(const char *key, uint32_t def);
int8_t config_read_item_i8(const char *key, int8_t def);
float config_read_item_f(const char *key, float def);

void config_setup_mqttClient();
struct cfg_mqtt* config_read_mqttClient(struct cfg_mqtt *c);
const char* config_read_mqtt_url(char *d, unsigned d_size);
const char* config_read_mqtt_user(char *d, unsigned d_size);
const char* config_read_mqtt_passwd(char *d, unsigned d_size);
const char* config_read_mqtt_client_id(char *d, unsigned d_size);
const char* config_read_mqtt_root_topic(char *d, unsigned d_size);
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

void config_setup_txtio(struct uo_flagsT *flags);
struct cfg_txtio* config_read_txtio(struct cfg_txtio *c);
enum verbosity config_read_verbose();

void config_setup_cliTcpServer(struct uo_flagsT *flags);

