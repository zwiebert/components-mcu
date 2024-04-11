/*
 * http_server.c
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */

#include "http_server_impl.h"
#include "net_http_server/http_server_setup.h"

void (*hts_register_uri_handlers_cb)(void *server_handle);

///////// public ///////////////////
void hts_setup(struct cfg_http *config) {
  hts_enable_http_server(config);
}


