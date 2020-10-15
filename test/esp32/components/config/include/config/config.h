/*
 * config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include "net/ethernet.h"


enum verbosity {
	vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};

#ifdef USE_NETWORK
enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};
#endif

typedef struct {
	enum verbosity app_verboseOutput;
	char app_configPassword[16];
#ifdef USE_SERIAL
	u32 mcu_serialBaud;
#endif
#ifdef USE_WLAN
    char wifi_SSID[32];
    char wifi_password[64];
#endif
#ifdef USE_HTTP
  char http_user[16];
  char http_password[31];
  i8 http_enable;
#endif
#ifdef USE_MQTT
  char mqtt_url[64];
  char mqtt_user[16];
  char mqtt_password[31];
  i8 mqtt_enable;
#endif
#ifdef USE_POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_NTP
  char ntp_server[64];
#endif
#ifdef USE_NETWORK
  enum nwConnection network;
#endif
#ifdef USE_LAN
  enum lanPhy lan_phy;
  int8_t lan_pwr_gpio;
#endif
  uint8_t stm32_inv_bootpin;

} config;

extern config C;



enum configItem {
  CB_VERBOSE,
#ifdef USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
  CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
#ifndef USE_POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
#ifdef USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  CB_NTP_SERVER,
#endif
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION, CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif
  CB_STM32_INV_BOOTPIN,

//-----------
  CB_size
};

// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);

void config_setup();

