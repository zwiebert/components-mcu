/**
 * \file   net/wifi_ap_setup.h
 * \brief  Configure WLAN access-point
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

#ifdef __cplusplus
  }
#endif
