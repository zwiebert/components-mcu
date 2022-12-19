/*
 * http_server.c
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#ifdef CONFIG_APP_USE_HTTP
#include "http_server_impl.h"
#include "net_http_server/http_server_setup.h"
#include <string.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "debug/dbg.h"

void (*hts_register_uri_handlers_cb)(void *server_handle);

///////// public ///////////////////
void hts_setup(struct cfg_http *config) {
  hts_enable_http_server(config);
}

#endif

