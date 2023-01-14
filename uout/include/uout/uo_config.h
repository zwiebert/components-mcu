#pragma once


class UoutWriter;

#ifdef CONFIG_APP_USE_LAN
void soCfg_LAN_PHY(const class UoutWriter &td);
void soCfg_LAN_PWR_GPIO(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_WLAN
void soCfg_WLAN_SSID(const class UoutWriter &td);
void soCfg_WLAN_PASSWORD(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_NTP
void soCfg_NTP_SERVER(const class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_MQTT
void soCfg_MQTT_ENABLE(const class UoutWriter &td);
void soCfg_MQTT_URL(const class UoutWriter &td);
void soCfg_MQTT_USER(const class UoutWriter &td);
void soCfg_MQTT_PASSWORD(const class UoutWriter &td);
void soCfg_MQTT_CLIENT_ID(const class UoutWriter &td);
void soCfg_MQTT_ROOT_TOPIC(const class UoutWriter &td);
#endif

#ifdef CONFIG_APP_USE_HTTP
void soCfg_HTTP_ENABLE(const class UoutWriter &td);
void soCfg_HTTP_USER(const class UoutWriter &td);
void soCfg_HTTP_PASSWORD(const class UoutWriter &td);
#endif
void soCfg_VERBOSE(const class UoutWriter &td);
