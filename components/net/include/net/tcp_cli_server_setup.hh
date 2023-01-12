/**
 * \file net/tcp_cli_server_setup.hh
 * \brief Create TCP server for interactive and non-interactive CLI clients
 * \author bertw
 */

#pragma once
#include "tcp_cli_server.h"
#include <uout/uo_callbacks.h>

/**
 * \brief Module Configuration
 */
struct cfg_tcps {
  uo_flagsT flags; ///< additional flags for callback
  bool enable = CONFIG_APP_TCPS_ENABLE;  ///< enable/disable CLI server task
};

/**
 * \brief            Start/stop server
 * \param cfg_tcps   configure object (will be copied)
 */
void tcpCli_setup(const struct cfg_tcps *cfg_tcps);

/**
 * \brief            Start/stop server as a task
 * \param cfg_tcps   configure object (will be copied)
 */
void tcpCli_setup_task(const struct cfg_tcps *cfg_tcps);


