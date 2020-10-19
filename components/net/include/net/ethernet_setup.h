/**
 * \file   ethernet_setup.h
 * \brief  Configure Ethernet hardware.
 */

#ifdef __cplusplus
  extern "C" {
#endif


#pragma once

#include <stdint.h>

/// \brief Kinds of supported PHY hardware.
enum lanPhy {
  lanPhyNone, lanPhyLAN8270, lanPhyRTL8201, lanPhyIP101, lanPhyLEN,
};

/// \brief Ethernet configuration data
struct cfg_lan {
  enum lanPhy phy;  ///< Select PHY hardware
  int8_t pwr_gpio;  ///< GPIO number of pin which controls the PHY power. Set to -1 if power if there is no such pin.
};

/**
 * \brief Configure Ethernet hardware
 */
void ethernet_setup(struct cfg_lan *config);

#ifdef __cplusplus
  }
#endif
