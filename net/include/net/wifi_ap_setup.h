/**
 * \file   net/wifi_ap_setup.h
 * \brief  Create WLAN access-point
 */


#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

  /**
   * \brief             Start WLAN AP
   * \param ap_ssid     SSID of the AP
   * \param pa_passwd   WAP2 Password to connect to the AP
   */
void wifiAp_setup(const char *ap_ssid, const char *ap_passwd);
void wifiAp_setdown();
int wifiAp_get_current_connections();
int wifiAp_get_connections_since_ap_start();
bool wifiAp_is_active();

#ifdef __cplusplus
  }
#endif
