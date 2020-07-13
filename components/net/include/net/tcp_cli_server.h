/*
 * tcp_cli_server.h
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#pragma once
#include "stdbool.h"

struct cfg_tcps { bool enable; };

void tcpCli_loop(void);
void tcpCli_setup(const struct cfg_tcps *cfg_tcps);

void tcpCli_setup_task(const struct cfg_tcps *cfg_tcps);

