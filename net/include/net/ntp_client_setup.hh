/**
 * \file   net/ntp_client_setup.hh
 * \brief  Create NTP client
 */

#pragma once

/// \brief NTP client configuration data
struct cfg_ntp {
  char server[64] = CONFIG_APP_NTP_SERVER; ///< NTP server to connect to. IP-address, or FQDN, or special key word "gateway"
};

/**
 *  \brief Start NTP client.
 *  \note  If the server is set to "gateway", the address of the gateway/router is used as NTP server address
 */
void ntp_setup(struct cfg_ntp *cfg_ntp);

