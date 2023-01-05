/**
 * \file   net_http_server/esp32/http_server_esp32.h
 * \brief  implementation specific interface for HTTP server
 */

#ifdef __cplusplus
  extern "C++" {
#endif
#pragma once

#include <esp_http_server.h>




extern httpd_handle_t hts_server; ///< Handle of the HTTP server if started. Or NULL if stopped.

/**
 * \brief      Test if a HTTP request is authenticated
 * \param req  An incoming HTTP request to authenticate
 * \return     true if authenticated, or if the server was created without authentication data
 */
bool check_access_allowed(struct httpd_req *req);

/**
* \brief                A callback in which the user registers his URI handlers
* \param server_handle  The HTTP-server to register the URI handlers
*/
extern void (*hts_register_uri_handlers_cb)(httpd_handle_t server_handle);


#ifdef __cplusplus
  }
#endif
