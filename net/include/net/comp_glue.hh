/**
 * \brief  optional header only glue inline functions, to glue components together.
 *         This header is supposed  to be included by an application.
 *
 *  To access this file, the caller need to have required modules: uout and config_kvs
 *  
 *
 */

#pragma once


#if __has_include(<config_kvs/settings.hh>) &&  __has_include(<uout/uout_writer.hh>)
#include <config_kvs/settings.hh>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config_kvs.h>
#include <app_config/options.hh>


#ifdef CONFIG_APP_USE_LAN
#include <net/ethernet_setup.hh>
inline struct cfg_lan* config_read_ethernet(struct cfg_lan *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_i8(h, CB_LAN_PHY, c->phy);
    kvsRead_i8(h, CB_LAN_PWR_GPIO, c->pwr_gpio);
    kvs_close(h);
  }
  return c;
}
inline void config_setup_ethernet() {
  struct cfg_lan c;
  config_read_ethernet(&c);
  ethernet_setup(&c);
}

#endif

#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
inline struct cfg_ntp* config_read_ntpClient(struct cfg_ntp *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_NTP_SERVER, c->server);
    kvs_close(h);
  }
  return c;
}
inline void config_setup_ntpClient() {
  struct cfg_ntp c;
  config_read_ntpClient(&c);
  ntp_setup(&c);
}
inline const char* config_read_ntp_server(char *d, unsigned d_size) {
  return config_read_item(CB_NTP_SERVER, d, d_size, cfg_ntp().server);
}
#endif

#ifdef CONFIG_APP_USE_WLAN
#include <net/wifi_station_setup.hh>
inline struct cfg_wlan *config_read_wifiStation(struct cfg_wlan *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_WIFI_SSID, c->SSID);
    kvsRead_charArray(h, CB_WIFI_PASSWD, c->password);
    kvs_close(h);
  }
  return c;
}
inline void config_setup_wifiStation() {
  struct cfg_wlan c;
  config_read_wifiStation(&c);
  wifistation_setup(&c);
}
inline const char* config_read_wifi_ssid(char *d, unsigned d_size) {
  return config_read_item(CB_WIFI_SSID, d, d_size, cfg_wlan().SSID);
}
inline const char* config_read_wifi_passwd(char *d, unsigned d_size) {
  return config_read_item(CB_WIFI_PASSWD, d, d_size, cfg_wlan().password);
}
#endif


#ifdef CONFIG_APP_USE_WLAN_AP
#include <net/wifi_ap_setup.h>
inline void config_setup_wifiAp() {
  wifiAp_setup(CONFIG_APP_WIFI_AP_SSID, CONFIG_APP_WIFI_AP_PASSWORD);
}
#endif

#if defined CONFIG_APP_USE_TCPS_TASK || defined CONFIG_APP_USE_TCPS
#include "net/tcp_cli_server.h"
#include "net/tcp_cli_server_setup.hh"
#include <uout/uo_callbacks.h>
inline void config_setup_cliTcpServer(struct uo_flagsT *flagsPtr) {
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

#include <uout/uout_writer.hh>

#ifdef CONFIG_APP_USE_LAN
#include "net/ethernet_setup.hh"
inline void soCfg_LAN_PHY(class UoutWriter &td) {
  cfg_lan c;
  td.so().print(comp_sett.get_optKeyStr(CB_LAN_PHY), cfg_args_lanPhy[config_read_ethernet(&c)->phy]);
}
inline void soCfg_LAN_PWR_GPIO(class UoutWriter &td) {
  cfg_lan c;
  td.so().print(comp_sett.get_optKeyStr(CB_LAN_PWR_GPIO), config_read_ethernet(&c)->pwr_gpio);
}
#endif
#ifdef CONFIG_APP_USE_WLAN
#include "net/wifi_station_setup.hh"
inline void soCfg_WLAN_SSID(class UoutWriter &td) {
  cfg_wlan c;
  td.so().print(comp_sett.get_optKeyStr(CB_WIFI_SSID), config_read_wifiStation(&c)->SSID);
}
inline void soCfg_WLAN_PASSWORD(class UoutWriter &td) {
  cfg_wlan c;
  td.so().print(comp_sett.get_optKeyStr(CB_WIFI_PASSWD), *config_read_wifiStation(&c)->password ? "*" : "");
}
#endif
#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
inline void soCfg_NTP_SERVER(class UoutWriter &td) {
  cfg_ntp c;
  td.so().print(comp_sett.get_optKeyStr(CB_NTP_SERVER), config_read_ntpClient(&c)->server);
}
#endif

template<typename settings_type>
constexpr void net_register_settings(settings_type &settings){
#ifdef CONFIG_APP_USE_WLAN
    settings.initField(CB_WIFI_SSID, "C_WIFI_SSID", otok::k_wlan_ssid, CBT_str, soCfg_WLAN_SSID, STF_direct);
    settings.initField(CB_WIFI_PASSWD, "C_WIFI_PASSWD", otok::k_wlan_password, CBT_str, soCfg_WLAN_PASSWORD, STF_direct);
#endif
#ifdef CONFIG_APP_USE_NTP
    settings.initField(CB_NTP_SERVER, "C_NTP_SERVER", otok::k_ntp_server, CBT_str, soCfg_NTP_SERVER, STF_direct);
#endif
#ifdef CONFIG_APP_USE_LAN
    settings.initField(CB_LAN_PHY, "C_LAN_PHY", otok::k_lan_phy, CBT_i8, soCfg_LAN_PHY);
    settings.initField(CB_LAN_PWR_GPIO, "C_LAN_PWR_GPIO", otok::k_lan_pwr_gpio, CBT_i8, soCfg_LAN_PWR_GPIO);
#endif
}

#endif
