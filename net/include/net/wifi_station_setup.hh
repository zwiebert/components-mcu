/**
 * \file    net/wifi_station_setup.hh
 * \brief   Create WLAN client
 */
#pragma once

/// \brief WLAN client configuration data
struct cfg_wlan {
#ifdef CONFIG_APP_USE_WLAN
  char SSID[32] = CONFIG_APP_WIFI_SSID; ///< SSID to connect to
  char password[64] = CONFIG_APP_WIFI_PASSWORD; ///< Password to connect with
#endif
};

/**
 * \brief  Try to connect to an access-point
 */
void wifistation_setup(struct cfg_wlan *config);
void wifistation_setdown();
bool wifistation_is_active();
