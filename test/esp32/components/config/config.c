/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#include "app/settings/config.h"
#include "config_defaults.h"

#define D(x) 

config C = {
#if 0
#ifdef USE_WLAN
  .wifi_SSID = MY_WIFI_SSID,
  .wifi_password = MY_WIFI_PASSWORD,
#endif
#ifdef USE_MQTT
  .mqtt_url = MY_MQTT_URL,
  .mqtt_user = MY_MQTT_USER,
  .mqtt_password = MY_MQTT_PASSWORD,
  MY_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  .http_user = MY_HTTP_USER,
  .http_password = MY_HTTP_PASSWORD,
  .http_enable = MY_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  .ntp_server = MY_NTP_SERVER,
#endif
#ifdef USE_POSIX_TIME
  .geo_tz = MY_GEO_TZ,
#endif
  .stm32_inv_bootpin = MY_STM32_INV_BOOTPIN,
#endif
};

void mcu_read_config(uint32_t mask);
void mcu_save_config(uint32_t mask);

void read_config(uint32_t mask) {
  mcu_read_config(mask);
}

void save_config(uint32_t mask) {
  mcu_save_config(mask);
}


void save_config_item(enum configItem item) {
  save_config(1UL << item);
}

void read_config_item(enum configItem item) {
  read_config(1UL << item);
}

void save_config_all() {
  save_config(~0UL);
}

void read_config_all() {
  read_config(~0UL);
}



