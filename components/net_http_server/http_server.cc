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

static bool isJson(const char *s, int s_len) {
  int i;
  for (i=0; i < s_len; ++i) {
    if (s[i] == ' ')
      continue;
    return s[i] == '{';
  }
  return false;
}

///////// public ///////////////////
void hts_setup(struct cfg_http *config) {
  hts_enable_http_server(config);
}

#endif

