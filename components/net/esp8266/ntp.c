/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <mem.h>

#include "config/config.h"
#include "app/rtc.h"
#include "config/config.h"
#include <sntp.h>
#include "net/ntp.h"
#include "txtio/inout.h"
#include "misc/int_types.h"

static time_t last_ntp_time;
bool ntp_initialized;
#define sntp_enabled() (ntp_initialized)

void ntp_setup(struct cfg_ntp *cfg_ntp) {
  if (sntp_enabled())
    sntp_stop();

  ip_addr_t *addr = (ip_addr_t*) os_zalloc(sizeof(ip_addr_t));
  if (strcmp(cfg_ntp->server, "gateway") == 0) {
    extern struct ip_addr ip4_gateway_address;
    sntp_setserver(0, &ip4_gateway_address);
    io_printf("gateway ntp: " IPSTR "\n", IP2STR(&ip4_gateway_address));
  } else if (ipaddr_aton(cfg_ntp->server, addr) > 0) {
    io_printf("ntp server address: " IPSTR "\n", IP2STR(addr));
    sntp_setserver(0, addr);
  } else {
    io_printf("ntp-server-name: %s\n", cfg_ntp->server);
    sntp_setservername(0, cfg_ntp->server);
  }
#if 0
    else {
      // example code
      sntp_setservername(0, "us.pool.ntp.org"); // set server 0 by domain name
      sntp_setservername(1, "ntp.sjtu.edu.cn"); // set server 1 by domain name
      ipaddr_aton("210.72.145.44", addr);
      sntp_setserver(2, addr); // set server 2 by IP address
    }
#endif
  sntp_set_timezone((int) 0);
  sntp_init();
  os_free(addr);
  ntp_initialized = true;

}

bool ntp_set_system_time(void) {
  if (!sntp_enabled())
    return false;

u32 time_stamp = sntp_get_current_timestamp();
if (time_stamp != 0) {
  time_t rtc_time, ntp_time;

  rtc_time = time(NULL);
  rtc_set_system_time(sntp_get_current_timestamp() - 946684800, RTC_SRC_NTP);
  ntp_time = time(NULL);

  io_printf_v(vrb1, "ntp stamp: %lu, %s (interval=%d, diff=%d)\n", sntp_get_current_timestamp(), sntp_get_real_time(sntp_get_current_timestamp()),
      (int) difftime(ntp_time, last_ntp_time), (int) (rtc_time - ntp_time));

  last_ntp_time = ntp_time;
  return true;
}
return false;
}

bool  ntp_update_system_time(unsigned interval_seconds) {
  if (!sntp_enabled())
    return false;

  if (last_ntp_time == 0 || time(NULL) >= last_ntp_time + interval_seconds) {
    ntp_set_system_time();
    return true;
  }
  return false;

}
