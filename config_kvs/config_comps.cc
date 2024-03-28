/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "config_kvs/config.h"
#include "config_kvs.h"
#include <txtio/txtio_setup.hh>
#include "config_kvs/comp_settings.hh"
#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "net/tcp_cli_server_setup.hh"
#include "uout/uo_callbacks.h"


#if defined CONFIG_APP_USE_TCPS_TASK || defined CONFIG_APP_USE_TCPS
#include "net/tcp_cli_server.h"
void config_setup_cliTcpServer(struct uo_flagsT *flagsPtr) {
  uo_flagsT flags = *flagsPtr;
  flags.evt.pin_change = true;
  flags.evt.gen_app_state_change = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;

  struct cfg_tcps c { .flags = flags, .enable = true };
#ifdef CONFIG_APP_USE_TCPS
 tcpCli_setup(&c);
#elif defined CONFIG_APP_USE_TCPS_TASK
 tcpCli_setup_task(&c);
#endif
}
#endif

#if 1
#include "txtio/inout.h"
struct cfg_txtio* config_read_txtio(struct cfg_txtio *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    int8_t verb = c->verbose;
    kvsRead_i8(h, CB_VERBOSE, verb);
    c->verbose = static_cast<verbosity>(verb);
    kvs_close(h);
  }
  return c;
}
void config_setup_txtio(struct uo_flagsT *flagsPtr) {
  uo_flagsT flags = *flagsPtr;
  flags.evt.pin_change = true;
  flags.evt.gen_app_state_change = true;
  flags.fmt.json = true;
  flags.fmt.txt = true;

  struct cfg_txtio c { .flags = flags, };
  config_read_txtio(&c);
  txtio_setup(&c);
}
enum verbosity config_read_verbose() {
  return static_cast<verbosity>(config_read_item(CB_VERBOSE, 3));
}
#endif

#ifdef CONFIG_STM32_USE_COMPONENT
#include <stm32/stm32.h>
cfg_stm32 *config_read_stm32(cfg_stm32 *cfg) {
  *cfg = cfg_stm32 { .uart_tx_gpio = CONFIG_STM32_UART_TX_PIN, .uart_rx_gpio = CONFIG_STM32_UART_RX_PIN, .boot_gpio_is_inverse =
      !!config_read_item((CB_STM32_INV_BOOTPIN), 0), .boot_gpio =  CONFIG_STM32_BOOT_PIN, .reset_gpio = CONFIG_STM32_RESET_PIN, };
  return cfg;
}
void config_setup_stm32() {
  struct cfg_stm32 c;
  config_read_stm32(&c);
  stm32_setup(&c);
}
#endif


#ifdef CONFIG_APP_USE_LAN
#include <net/ethernet_setup.hh>
struct cfg_lan* config_read_ethernet(struct cfg_lan *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_i8(h, CB_LAN_PHY, c->phy);
    kvsRead_i8(h, CB_LAN_PWR_GPIO, c->pwr_gpio);
    kvs_close(h);
  }
  return c;
}
void config_setup_ethernet() {
  struct cfg_lan c;
  config_read_ethernet(&c);
  ethernet_setup(&c);
}

#endif

#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
struct cfg_ntp* config_read_ntpClient(struct cfg_ntp *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_NTP_SERVER, c->server);
    kvs_close(h);
  }
  return c;
}
void config_setup_ntpClient() {
  struct cfg_ntp c;
  config_read_ntpClient(&c);
  ntp_setup(&c);
}
const char* config_read_ntp_server(char *d, unsigned d_size) {
  return config_read_item(CB_NTP_SERVER, d, d_size, cfg_ntp().server);
}
#endif

#ifdef CONFIG_APP_USE_WLAN
#include <net/wifi_station_setup.hh>
struct cfg_wlan *config_read_wifiStation(struct cfg_wlan *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_WIFI_SSID, c->SSID);
    kvsRead_charArray(h, CB_WIFI_PASSWD, c->password);
    kvs_close(h);
  }
  return c;
}
void config_setup_wifiStation() {
  struct cfg_wlan c;
  config_read_wifiStation(&c);
  wifistation_setup(&c);
}
const char* config_read_wifi_ssid(char *d, unsigned d_size) {
  return config_read_item(CB_WIFI_SSID, d, d_size, cfg_wlan().SSID);
}
const char* config_read_wifi_passwd(char *d, unsigned d_size) {
  return config_read_item(CB_WIFI_PASSWD, d, d_size, cfg_wlan().password);
}
#endif

#ifdef CONFIG_APP_USE_MQTT
#include "net_mqtt/mqtt.hh"
struct cfg_mqtt* config_read_mqttClient(struct cfg_mqtt *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_MQTT_URL, c->url);
    kvsRead_charArray(h, CB_MQTT_USER, c->user);
    kvsRead_charArray(h, CB_MQTT_PASSWD, c->password);
    kvsRead_charArray(h, CB_MQTT_CLIENT_ID, c->client_id);
    kvsRead_charArray(h, CB_MQTT_ROOT_TOPIC, c->root_topic);
    kvsRead_i8(h, CB_MQTT_ENABLE, c->enable);
    kvs_close(h);
  }
  return c;
}
void config_setup_mqttClient(struct cfg_mqtt *cp) {
  struct cfg_mqtt c;
  config_read_mqttClient(&c);
  io_mqtt_setup(&c);
}

#endif

#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
struct cfg_http* config_read_httpServer(struct cfg_http *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_HTTP_USER, c->user);
    kvsRead_charArray(h, CB_HTTP_PASSWD, c->password);
    kvsRead_i8(h, CB_HTTP_ENABLE, c->enable);

    kvs_close(h);
  }
  return c;
}
void config_setup_httpServer() {
  struct cfg_http c;
  config_read_httpServer(&c);
  hts_setup(&c);
}

bool config_read_http_enable() {
  return !!config_read_item(CB_HTTP_ENABLE, cfg_http().enable);
}
#endif

#ifdef CONFIG_STM32_USE_COMPONENT
bool config_read_stm32_inv_bootpin() {
  return !!config_read_item((CB_STM32_INV_BOOTPIN), 0);
}
#endif
