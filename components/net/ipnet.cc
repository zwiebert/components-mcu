/*
 * wifi.c
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "net/ipnet.h"
#include "net_http_server/http_server_setup.h"
#include "utils_misc/int_types.h"

extern "C++" void main_setup_ip_dependent(void);
uint32_t ip4_address, ip4_gateway_address, ip4_netmask;

uint32_t get_ip4addr_local();
uint32_t get_ip4addr_gateway();
uint32_t get_ip4netmask();

void (*ipnet_CONNECTED_cb)();


static bool Is_connected;

void ipnet_connected(void) {
  Is_connected = true;
if(ipnet_CONNECTED_cb)
  ipnet_CONNECTED_cb();
}

void ipnet_disconnected(void) {
  Is_connected = false;
#ifdef CONFIG_APP_USE_HTTP
  hts_setup(0);
#endif
}

bool ipnet_isConnected(void) {
  return Is_connected;
}
