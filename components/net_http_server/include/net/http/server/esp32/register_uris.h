#ifdef __cplusplus
  extern "C++" {
#endif
#pragma once

#include <esp_http_server.h>

extern httpd_handle_t hts_server;
void hts_register_uri_handlers(httpd_handle_t server); // IMPLEMENT_ME


#ifdef __cplusplus
  }
#endif
