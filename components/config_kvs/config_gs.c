#include "config_kvs/config.h"

#ifdef USE_MQTT
const char* config_read_mqtt_url(char *d, unsigned d_size) {
  return config_read_item_s(CB_MQTT_URL, d, d_size, MY_MQTT_URL);
}

const char* config_read_mqtt_user(char *d, unsigned d_size) {
  return config_read_item_s(CB_MQTT_USER, d, d_size, MY_MQTT_USER);
}
const char* config_read_mqtt_passwd(char *d, unsigned d_size) {
  return config_read_item_s(CB_MQTT_PASSWD, d, d_size, MY_MQTT_PASSWORD);
}
const char* config_read_mqtt_client_id(char *d, unsigned d_size) {
  return config_read_item_s(CB_MQTT_CLIENT_ID, d, d_size, MY_MQTT_CLIENT_ID);
}

bool config_read_mqtt_enable() {
  return !!config_read_item_i8(CB_MQTT_ENABLE, MY_MQTT_ENABLE);
}
#endif

#ifdef USE_HTTP
const char* config_read_http_user(char *d, unsigned d_size) {
  return config_read_item_s(CB_HTTP_USER, d, d_size, MY_HTTP_USER);
}
const char* config_read_http_passwd(char *d, unsigned d_size) {
  return config_read_item_s(CB_HTTP_PASSWD, d, d_size, MY_HTTP_PASSWORD);
}

bool config_read_http_enable() {
  return !!config_read_item_i8(CB_HTTP_ENABLE, MY_HTTP_ENABLE);
}
#endif

#ifdef USE_WLAN
const char* config_read_wifi_ssid(char *d, unsigned d_size) {
  return config_read_item_s(CB_WIFI_SSID, d, d_size, MY_WIFI_SSID);
}
const char* config_read_wifi_passwd(char *d, unsigned d_size) {
  return config_read_item_s(CB_WIFI_PASSWD, d, d_size, MY_WIFI_PASSWORD);
}
#endif


const char* config_read_ntp_server(char *d, unsigned d_size) {
  return config_read_item_s(CB_NTP_SERVER, d, d_size, MY_NTP_SERVER);
}

#ifdef USE_LAN
int8_t config_read_lan_phy() {
  return config_read_item_i8(CB_LAN_PHY, MY_LAN_PHY);
}
int8_t config_read_lan_pwr_gpio() {
  return config_read_item_i8(CB_LAN_PWR_GPIO, MY_LAN_PWR_GPIO);
}
#endif

enum verbosity config_read_verbose() {
  return config_read_item_i8(CB_VERBOSE, MY_VERBOSE);
}



