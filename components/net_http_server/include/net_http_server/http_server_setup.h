/**
 * \file   net_http_server/http_server_setup.h
 * \brief  Create HTTP server
 */

#ifdef __cplusplus
  extern "C++" {
#endif

#pragma once



#include "stdbool.h"
#include <stdint.h>



/// \brief HTTP server configuration data
struct cfg_http {
  char user[16];  ///< Auth user-name or empty string for no auth
  char password[31]; ///< Auth password or empty string for no auth
  int8_t enable;  ///< If true, start server. If false, stop server.
};


/**
 * \brief Start/stop HTTP server
 */
void hts_setup(struct cfg_http *config);

#ifdef __cplusplus
  }
#endif
