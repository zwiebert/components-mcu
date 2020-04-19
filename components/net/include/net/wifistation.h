/*
 * wifistation.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#pragma once

struct cfg_wlan {
  char SSID[32];
  char password[64];
};

void wifistation_setup(struct cfg_wlan *config);

typedef void (*wifistation_cb)(void);
void wifistation_cbRegister_gotIpAddr(wifistation_cb cb);
void wifistation_cbRegister_lostIpAddr(wifistation_cb cb);

