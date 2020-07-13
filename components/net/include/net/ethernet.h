/*
 * ethernet.h
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>

enum lanPhy {
  lanPhyNone, lanPhyLAN8270, lanPhyRTL8201, lanPhyIP101, lanPhyLEN,
};

struct cfg_lan {
  enum lanPhy phy;
  int8_t pwr_gpio;
};

void ethernet_configure(enum lanPhy lan_phy, int lan_pwr_gpio);
void ethernet_setup(struct cfg_lan *config);


