#pragma once
#include "app_config/proj_app_cfg.h"

struct TargetDesc;

#ifdef USE_LAN
void soCfg_LAN_PHY(const struct TargetDesc &td);
void soCfg_LAN_PWR_GPIO(const struct TargetDesc &td);
#endif
#ifdef USE_WLAN
void soCfg_WLAN_SSID(const struct TargetDesc &td);
void soCfg_WLAN_PASSWORD(const struct TargetDesc &td);
#endif
#ifdef USE_NTP
void soCfg_NTP_SERVER(const struct TargetDesc &td);
#endif
#ifdef USE_MQTT
void soCfg_MQTT_ENABLE(const struct TargetDesc &td);
void soCfg_MQTT_URL(const struct TargetDesc &td);
void soCfg_MQTT_USER(const struct TargetDesc &td);
void soCfg_MQTT_PASSWORD(const struct TargetDesc &td);
void soCfg_MQTT_CLIENT_ID(const struct TargetDesc &td);
void soCfg_MQTT_ROOT_TOPIC(const struct TargetDesc &td);
#endif

#ifdef USE_HTTP
void soCfg_HTTP_ENABLE(const struct TargetDesc &td);
void soCfg_HTTP_USER(const struct TargetDesc &td);
void soCfg_HTTP_PASSWORD(const struct TargetDesc &td);
#endif
void soCfg_VERBOSE(const struct TargetDesc &td);
