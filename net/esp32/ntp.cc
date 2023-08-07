/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include <ctype.h>
#include <lwip/ip_addr.h>
#include <esp32/rom/ets_sys.h>
#include <string.h>



#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
#include "esp_event.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_sntp.h"

#define printf ets_printf
#define D(x)

extern ip_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

static void set_server_by_config(struct cfg_ntp *cfg_ntp) {
  int server_number = 0;
  ip_addr_t addr[SNTP_MAX_SERVERS];

#if SNTP_MAX_SERVERS > 1
  static char servers[sizeof C.ntp.server];

  memcpy(servers,C.ntp.server, sizeof servers);
  const char *server = servers;
  char *sep[SNTP_MAX_SERVERS-1];

  int i;

  for (i=0; i < SNTP_MAX_SERVERS-1; ++i) {
    if ((sep[i] = strchr((i == 0 ? server : sep[i-1]+1), ','))) {
      *sep[i] = '\0';
    } else {
      break;
    }
  }

  for (server_number = 0; server_number < SNTP_MAX_SERVERS; ++server_number) {
    if (server_number != 0) {
      if (sep[server_number - 1])
      server = sep[server_number - 1] + 1;
      else
      break;
    }
#else
  {
    const char *server = cfg_ntp->server;
#endif
    bool use_dhcp = strcmp(server, "dhcp") == 0;
    bool use_gateway = strcmp(server, "gateway") == 0;
    bool use_ipaddr = isdigit((int )server[0]);

    esp_sntp_servermode_dhcp(use_dhcp);

    if (use_gateway) {
      esp_sntp_setserver(server_number, &ip4_gateway_address);
    } else if (use_ipaddr) {
      ipaddr_aton(server, &addr[server_number]);
      esp_sntp_setserver(server_number, &addr[server_number]);
    } else if (use_dhcp) {
#if SNTP_MAX_SERVERS > 1
      break;
#endif
    } else {
#if SNTP_MAX_SERVERS > 1
      sntp_setservername(server_number, server);
#else
      static char *server_name;
      free(server_name);
      server_name = strdup(server);
      esp_sntp_setservername(0, server_name);
#endif
    }
  }
}


void ntp_setup(struct cfg_ntp *cfg_ntp) {
    esp_sntp_stop();
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    set_server_by_config(cfg_ntp);
    esp_sntp_init();
    D(ets_printf("server:<%s> <%s> <%s>\n",sntp_getservername(0), sntp_getservername(1), sntp_getservername(2)));
}

#endif

