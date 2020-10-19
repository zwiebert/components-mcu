/**
 * \file net/tcp_cli_server.h
 * \brief TCP server for interactive and non-interactive CLI clients
 * \author bertw
 */

#ifdef __cplusplus
  extern "C" {
#endif

#pragma once
#include "stdbool.h"


/**
 * \brief  Do work
 * \note   Not implemented if the server runs as a separate task
 */
void tcpCli_loop(void);


#ifdef __cplusplus
  }
#endif
