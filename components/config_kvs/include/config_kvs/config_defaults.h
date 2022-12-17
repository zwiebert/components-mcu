#include "app_config/proj_app_cfg.h"
#ifdef __cplusplus
  extern "C++" {
#endif
/*
 * config_defaults.h
 *
 *  Created on: 19.03.2020
 *      Author: bertw
 */

#pragma once

#if __has_include("sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#endif

#if defined HAVE_USER_DEFAULTS && ! defined DISTRIBUTION
#include "sensitive/defaults.h"
#endif

#ifndef MY_MCU_UART_BAUD_RATE
#ifdef CONFIG_MY_MCU_UART_BAUD_RATE
#define MY_MCU_UART_BAUD_RATE CONFIG_MY_MCU_UART_BAUD_RATE
#else
#define MY_MCU_UART_BAUD_RATE 115200
#endif
#endif

#ifndef MY_WIFI_SSID
#ifdef CONFIG_MY_WIFI_SSID
#define MY_WIFI_SSID CONFIG_MY_WIFI_SSID
#else
#define MY_WIFI_SSID ""
#endif
#endif

#ifndef MY_WIFI_PASSWORD
#ifdef CONFIG_MY_WIFI_PASSWORD
#define MY_WIFI_PASSWORD CONFIG_MY_WIFI_PASSWORD
#else
#define MY_WIFI_PASSWORD ""
#endif
#endif

#ifndef MY_NTP_SERVER
#ifdef CONFIG_MY_NTP_SERVER
#define MY_NTP_SERVER CONFIG_MY_NTP_SERVER
#else
#define MY_NTP_SERVER "gateway"
#endif
#endif

#ifndef MY_MQTT_ENABLE
#ifdef CONFIG_MY_MQTT_ENABLE
#define MY_MQTT_ENABLE CONFIG_MY_MQTT_ENABLE
#else
#define MY_MQTT_ENABLE 0
#endif
#endif

#ifndef MY_MQTT_URL
#ifdef CONFIG_MY_MQTT_URL
#define MY_MQTT_URL CONFIG_MY_MQTT_URL
#else
#define MY_MQTT_URL ""
#endif
#endif

#ifndef MY_MQTT_USER
#ifdef CONFIG_MY_MQTT_USER
#define MY_MQTT_USER CONFIG_MY_MQTT_USER
#else
#define MY_MQTT_USER ""
#endif
#endif

#ifndef MY_MQTT_PASSWORD
#ifdef CONFIG_MY_MQTT_PASSWORD
#define MY_MQTT_PASSWORD CONFIG_MY_MQTT_PASSWORD
#else
#define MY_MQTT_PASSWORD ""
#endif
#endif

#ifndef MY_MQTT_CLIENT_ID
#ifdef CONFIG_MY_MQTT_CLIENT_ID
#define MY_MQTT_CLIENT_ID CONFIG_MY_MQTT_CLIENT_ID
#else
#define MY_MQTT_CLIENT_ID "my_mcu"
#endif
#endif

#ifndef MY_MQTT_ROOT_TOPIC
#ifdef CONFIG_MY_MQTT_ROOT_TOPIC
#define MY_MQTT_ROOT_TOPIC CONFIG_MY_MQTT_ROOT_TOPIC
#else
#define MY_MQTT_ROOT_TOPIC "my_mcu"
#endif
#endif

#ifndef MY_HTTP_ENABLE
#ifdef CONFIG_MY_HTTP_ENABLE
#define MY_HTTP_ENABLE CONFIG_MY_HTTP_ENABLE
#else
#define MY_HTTP_ENABLE 0
#endif
#endif

#ifndef MY_HTTP_USER
#ifdef CONFIG_MY_HTTP_USER
#define MY_HTTP_USER CONFIG_MY_HTTP_USER
#else
#define MY_HTTP_USER ""
#endif
#endif

#ifndef MY_HTTP_PASSWORD
#ifdef CONFIG_MY_HTTP_PASSWORD
#define MY_HTTP_PASSWORD CONFIG_MY_HTTP_PASSWORD
#else
#define MY_HTTP_PASSWORD ""
#endif
#endif

#ifndef MY_NETWORK_CONNECTION
#ifdef USE_LAN
#define MY_NETWORK_CONNECTION nwLan
#else
#define MY_NETWORK_CONNECTION nwWlanAp
#endif
#endif
#define MY_NETWORK_CONNECTION_OLD_USERS nwWlanSta

#ifndef MY_LAN_PHY
#define MY_LAN_PHY lanPhyLAN8720
#endif

#ifndef MY_LAN_PWR_GPIO
#define MY_LAN_PWR_GPIO -1
#endif

#ifdef __cplusplus
  }
#endif
