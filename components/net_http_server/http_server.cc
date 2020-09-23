/*
 * http_server.c
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#ifdef USE_HTTP
#include "net/http/server/http_server.h"
#include <string.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "debug/dbg.h"

void (*ws_print_json_cb)(const char *json);
void (*hts_register_uri_handlers_cb)(void *server_handle);

///////// public ///////////////////
void hts_setup(struct cfg_http *config) {
  hts_enable_http_server(config);
}

#endif

