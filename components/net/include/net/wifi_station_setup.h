/**
 * \file    net/wifi_station_setup.h
 * \brief   Create WLAN client
 */
#ifdef __cplusplus
  extern "C" {
#endif


#pragma once

/// \brief WLAN client configuration data
struct cfg_wlan {
  char SSID[32]; ///< SSID to connect to
  char password[64]; ///< Password to connect with
};

/**
 * \brief  Try to connect to an access-point
 */
void wifistation_setup(struct cfg_wlan *config);

#ifdef __cplusplus
  }
#endif
