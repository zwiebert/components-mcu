/*
 * ntp.h
 *
 *  Created on: 21.03.2020
 *      Author: bertw
 */

#pragma once

struct cfg_ntp { char server[64]; };

void ntp_setup(struct cfg_ntp *cfg_ntp);


