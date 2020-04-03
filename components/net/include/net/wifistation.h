/*
 * wifistation.h
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#ifndef USER_ESP32_MAIN_WIFISTATION_H_
#define USER_ESP32_MAIN_WIFISTATION_H_

struct cfg_wlan {
  char SSID[32];
  char password[64];
};

void wifistation_setup(struct cfg_wlan *config);

typedef void (*wifistation_cb)(void);
void wifistation_cbRegister_gotIpAddr(wifistation_cb cb);
void wifistation_cbRegister_lostIpAddr(wifistation_cb cb);

#endif /* USER_ESP32_MAIN_WIFISTATION_H_ */
