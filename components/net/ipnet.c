/*
 * wifi.c
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "net/ipnet.h"
#include "net/http/server/http_server.h"
#include "misc/int_types.h"

void main_setup_ip_dependent(void);
u32 ip4_address, ip4_gateway_address, ip4_netmask;

u32 get_ip4addr_local();
u32 get_ip4addr_gateway();
u32 get_ip4netmask();


static bool Is_connected;

void ipnet_connected(void) {
  Is_connected = true;
 //XXX soMsg_INET_PRINT_ADDRESS();
  main_setup_ip_dependent();
}

void ipnet_disconnected(void) {
  Is_connected = false;
#ifdef USE_HTTP
  hts_setup(0);
#endif
}

bool ipnet_isConnected(void) {
  return Is_connected;
}
