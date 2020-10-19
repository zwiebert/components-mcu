/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"
#include "config_kvs.h"
#include <txtio/txtio_setup.hh>

#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include <app_uout/callbacks.h>
#include "net/tcp_cli_server_setup.hh"


#if defined USE_TCPS_TASK || defined USE_TCPS
#include "net/tcp_cli_server.h"
void config_setup_cliTcpServer() {
  uo_flagsT flags;
  flags.evt.pin_change = true;
  flags.evt.pct_change = true;
  flags.evt.rf_msg_received = true;
  flags.evt.uo_evt_flag_CUAS = true;
  flags.evt.uo_evt_flag_PRAS = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;

  struct cfg_tcps c { .flags = flags, .enable = true };
#ifdef USE_TCPS
 tcpCli_setup(&c);
#elif defined USE_TCPS_TASK
 tcpCli_setup_task(&c);
#endif
}
#endif

#if 1
#include "txtio/inout.h"
struct cfg_txtio* config_read_txtio(struct cfg_txtio *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    i8 verb = c->verbose;
    kvsR(i8, CB_VERBOSE, verb);
    c->verbose = static_cast<verbosity>(verb);
    kvs_close(h);
  }
  return c;
}
void config_setup_txtio() {
  uo_flagsT flags;
  flags.evt.pin_change = true;
  flags.evt.pct_change = true;
  flags.evt.rf_msg_received = true;
  flags.evt.uo_evt_flag_CUAS = true;
  flags.evt.uo_evt_flag_PRAS = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;

  struct cfg_txtio c { .flags = flags, .baud = MY_MCU_UART_BAUD_RATE };
  config_read_txtio(&c);
  txtio_setup(&c);
}
enum verbosity config_read_verbose() {
  return static_cast<verbosity>(config_read_item_i8(CB_VERBOSE, 0));
}
#endif

#ifdef USE_LAN
#include "net/ethernet.h"
struct cfg_lan* config_read_ethernet(struct cfg_lan *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRead(i8, lanPhy, CB_LAN_PHY, c->phy);
    kvsR(i8, CB_LAN_PWR_GPIO, c->pwr_gpio);
    kvs_close(h);
  }
  return c;
}
void config_setup_ethernet() {
  struct cfg_lan c = { .phy = MY_LAN_PHY, .pwr_gpio = MY_LAN_PWR_GPIO, };
  config_read_ethernet(&c);
  ethernet_setup(&c);
}
int8_t config_read_lan_phy() {
  return config_read_item_i8(CB_LAN_PHY, MY_LAN_PHY);
}
int8_t config_read_lan_pwr_gpio() {
  return config_read_item_i8(CB_LAN_PWR_GPIO, MY_LAN_PWR_GPIO);
}
#endif

#ifdef USE_NTP
#include "net/ntp.h"
struct cfg_ntp* config_read_ntpClient(struct cfg_ntp *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_NTP_SERVER, c->server);
    kvs_close(h);
  }
  return c;
}
void config_setup_ntpClient() {
  struct cfg_ntp c { {.server = MY_NTP_SERVER} };
  config_read_ntpClient(&c);
  ntp_setup(&c);
}
const char* config_read_ntp_server(char *d, unsigned d_size) {
  return config_read_item_s(CB_NTP_SERVER, d, d_size, MY_NTP_SERVER);
}
#endif

#ifdef USE_WLAN
#include "net/wifistation.h"
struct cfg_wlan *config_read_wifiStation(struct cfg_wlan *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_WIFI_SSID, c->SSID);
    kvsRs(CB_WIFI_PASSWD, c->password);
    kvs_close(h);
  }
  return c;
}
void config_setup_wifiStation() {
  struct cfg_wlan c { {.SSID = MY_WIFI_SSID}, {.password = MY_WIFI_PASSWORD}, };
  config_read_wifiStation(&c);
  wifistation_setup(&c);
}
const char* config_read_wifi_ssid(char *d, unsigned d_size) {
  return config_read_item_s(CB_WIFI_SSID, d, d_size, MY_WIFI_SSID);
}
const char* config_read_wifi_passwd(char *d, unsigned d_size) {
  return config_read_item_s(CB_WIFI_PASSWD, d, d_size, MY_WIFI_PASSWORD);
}
#endif

#ifdef USE_MQTT
#include "net_mqtt/mqtt.h"
struct cfg_mqtt* config_read_mqttClient(struct cfg_mqtt *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_MQTT_URL, c->url);
    kvsRs(CB_MQTT_USER, c->user);
    kvsRs(CB_MQTT_PASSWD, c->password);
    kvsRs(CB_MQTT_CLIENT_ID, c->client_id);
    kvsR(i8, CB_MQTT_ENABLE, c->enable);
    kvs_close(h);
  }
  return c;
}
void config_setup_mqttClient() {
  struct cfg_mqtt c { {.url = MY_MQTT_URL}, {.user = MY_MQTT_USER}, {.password = MY_MQTT_PASSWORD}, { .client_id = MY_MQTT_CLIENT_ID},
    MY_MQTT_ENABLE };
  config_read_mqttClient(&c);
  io_mqtt_setup(&c);
}
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
#include "net_http_server/http_server.h"
struct cfg_http* config_read_httpServer(struct cfg_http *c) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_HTTP_USER, c->user);
    kvsRs(CB_HTTP_PASSWD, c->password);
    kvsR(i8, CB_HTTP_ENABLE, c->enable);

    kvs_close(h);
  }
  return c;
}
void config_setup_httpServer() {
  struct cfg_http c { {.user = MY_HTTP_USER}, {.password = MY_HTTP_PASSWORD }, MY_HTTP_ENABLE };
  config_read_httpServer(&c);
  hts_setup(&c);
}
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
