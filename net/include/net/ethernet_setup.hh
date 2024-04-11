/**
 * \file   ethernet_setup.hh
 * \brief  Configure Ethernet hardware.
 */

#pragma once

#include <stdint.h>

/// \brief Kinds of supported PHY hardware.
enum lanPhy {
  lanPhyNone, lanPhyLAN8720, lanPhyRTL8201, lanPhyIP101, lanPhyLEN,
};
#define CONFIG_APP_LAN_PHY lanPhyLAN8720
#define CONFIG_APP_LAN_PWR_GPIO -1
/// \brief Ethernet configuration data
struct cfg_lan {
  enum lanPhy phy = CONFIG_APP_LAN_PHY;  ///< Select PHY hardware
  int8_t pwr_gpio = CONFIG_APP_LAN_PWR_GPIO;  ///< GPIO number of pin which controls the PHY power. Set to -1 if power if there is no such pin.
};

/**
 * \brief Configure Ethernet hardware
 */
void ethernet_setup(struct cfg_lan *config);

void ethernet_setdown();
