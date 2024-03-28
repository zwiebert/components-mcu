#pragma once


class UoutWriter;

#ifdef CONFIG_APP_USE_LAN
void soCfg_LAN_PHY(class UoutWriter &td);
void soCfg_LAN_PWR_GPIO(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_WLAN
void soCfg_WLAN_SSID(class UoutWriter &td);
void soCfg_WLAN_PASSWORD(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_NTP
void soCfg_NTP_SERVER(class UoutWriter &td);
#endif
#ifdef CONFIG_APP_USE_MQTT
void soCfg_MQTT_ENABLE(class UoutWriter &td);
void soCfg_MQTT_URL(class UoutWriter &td);
void soCfg_MQTT_USER(class UoutWriter &td);
void soCfg_MQTT_PASSWORD(class UoutWriter &td);
void soCfg_MQTT_CLIENT_ID(class UoutWriter &td);
void soCfg_MQTT_ROOT_TOPIC(class UoutWriter &td);
#endif

#ifdef CONFIG_APP_USE_HTTP
void soCfg_HTTP_ENABLE(class UoutWriter &td);
void soCfg_HTTP_USER(class UoutWriter &td);
void soCfg_HTTP_PASSWORD(class UoutWriter &td);
#endif
void soCfg_VERBOSE(class UoutWriter &td);
#ifdef CONFIG_STM32_USE_COMPONENT
void soCfg_STM32_BOOTGPIO_INV(class UoutWriter &td);
#endif
