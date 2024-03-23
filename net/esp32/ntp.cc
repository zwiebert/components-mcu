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
#ifdef CONFIG_NET_NTP_CLIENT_DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "net_ntp_client"


extern ip_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

static void set_server_by_config(struct cfg_ntp *cfg_ntp) {
  ip_addr_t addr[SNTP_MAX_SERVERS];

#if SNTP_MAX_SERVERS > 1
  static char servers[sizeof cfg_ntp->server];
  memcpy(servers,cfg_ntp->server, sizeof servers);
  const char *server = servers;
  char *sep[SNTP_MAX_SERVERS-1];

  for (int i=0; i < SNTP_MAX_SERVERS-1; ++i) {
    if ((sep[i] = strchr((i == 0 ? server : sep[i-1]+1), ','))) {
      *sep[i] = '\0';
    } else {
      break;
    }
  }
#endif

  for (int server_number = 0; server_number < SNTP_MAX_SERVERS; ++server_number) {
#if SNTP_MAX_SERVERS > 1
    if (server_number != 0) {
      if (sep[server_number - 1])
      server = sep[server_number - 1] + 1;
      else
      break;
    }
#else
    const char *server = cfg_ntp->server;
#endif

    // Get NTP server from DHCP
#ifdef  LWIP_DHCP_GET_NTP_SRV
    bool use_dhcp = strcmp(server, "dhcp") == 0;
    esp_sntp_servermode_dhcp(use_dhcp);

    if (use_dhcp) {
      if (SNTP_MAX_SERVERS > 1)
        break;
      continue;
    }
#endif

    // Get NTP server from gateway router
    if (0 == strcmp(server, "gateway")) {
      esp_sntp_setserver(server_number, &ip4_gateway_address);
      continue;
    }

    // Get NTP server from IP4 address
    if (isdigit((int) server[0])) {
      ipaddr_aton(server, &addr[server_number]);
      esp_sntp_setserver(server_number, &addr[server_number]);
      continue;
    }

#if SNTP_MAX_SERVERS > 1
      esp_sntp_setservername(server_number, server);
#else
    static char *server_name;
    free(server_name);
    server_name = strdup(server);
    esp_sntp_setservername(0, server_name);
#endif

  }
}


void ntp_setup(struct cfg_ntp *cfg_ntp) {
  esp_sntp_stop();
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  set_server_by_config(cfg_ntp);
  esp_sntp_init();
}

#endif

