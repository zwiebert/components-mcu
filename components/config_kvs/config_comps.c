/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "config_kvs/config.h"
#include "config_kvs.h"

#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"


#if defined USE_TCPS_TASK || defined USE_TCPS
#include "net/tcp_cli_server.h"
void config_setup_cliTcpServer() {
  struct cfg_tcps c = { .enable = true };
#if 0
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_VERBOSE, c.verbose);
    kvs_close(h);
  }
#endif
#ifdef USE_TCPS
 tcpCli_setup(&c);
#elif defined USE_TCPS_TASK
 tcpCli_setup_task(&c);
#endif
}
#endif

#if 1
#include "txtio/inout.h"
void config_setup_txtio() {
  kvshT h;
  struct cfg_txtio c = { .verbose = MY_VERBOSE };

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_VERBOSE, c.verbose);
    kvs_close(h);
  }
 txtio_setup(&c);
}
#endif

#ifdef USE_LAN
#include "net/ethernet.h"
void config_setup_ethernet() {
  kvshT h;
  struct cfg_lan c = { .phy = MY_LAN_PHY, .pwr_gpio = MY_LAN_PWR_GPIO, };

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_LAN_PHY, c.phy);
    kvsR(i8, CB_LAN_PWR_GPIO, c.pwr_gpio);
    kvs_close(h);
  }
  ethernet_setup(&c);
}
#endif

#ifdef USE_NTP
#include "net/ntp.h"
void config_setup_ntpClient() {
  kvshT h;
  struct cfg_ntp c = { .server = MY_NTP_SERVER };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_NTP_SERVER, c.server);
    kvs_close(h);
  }
  ntp_setup(&c);
}
#endif

#ifdef USE_WLAN
#include "net/wifistation.h"
void config_setup_wifiStation() {
  kvshT h;
  struct cfg_wlan c = { .SSID = MY_WIFI_SSID, .password = MY_WIFI_PASSWORD, };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_WIFI_SSID, c.SSID);
    kvsRs(CB_WIFI_PASSWD, c.password);
    kvs_close(h);
  }
  wifistation_setup(&c);
}
#endif

#ifdef USE_MQTT
#include "net/mqtt/mqtt.h"
void config_setup_mqttClient() {
  kvshT h;
  struct cfg_mqtt c = { .url = MY_MQTT_URL, .user = MY_MQTT_USER, .password = MY_MQTT_PASSWORD, .client_id = MY_MQTT_CLIENT_ID, .enable =
  MY_MQTT_ENABLE };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_MQTT_URL, c.url);
    kvsRs(CB_MQTT_USER, c.user);
    kvsRs(CB_MQTT_PASSWD, c.password);
    kvsRs(CB_MQTT_CLIENT_ID, c.client_id);
    kvsR(i8, CB_MQTT_ENABLE, c.enable);
    kvs_close(h);
  }
  io_mqtt_setup(&c);
}
#endif

#ifdef USE_HTTP
#include "net/http/server/http_server.h"
void config_setup_httpServer() {
  kvshT h;
  struct cfg_http c = { .user = MY_HTTP_USER, .password = MY_HTTP_PASSWORD, .enable = MY_HTTP_ENABLE };
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsRs(CB_HTTP_USER, c.user);
    kvsRs(CB_HTTP_PASSWD, c.password);
    kvsR(i8, CB_HTTP_ENABLE, c.enable);

    kvs_close(h);
  }
  hts_setup(&c);
}
#endif
