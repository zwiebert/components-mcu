/*
 * cmdline_process_cli.c
 *
 *  Created on: 18.03.2020
 *      Author: bertw
 */

#include "cli_private.h"
#include "utils_misc/bcd.h"
#include "cli/cli.h"
#include "uout/uo_types.h"
#include "uout/uo_callbacks.h"
#include "uout/cli_out.h"
//#include "net_http_server/http_server_setup.h"
#include "txtio/inout.h"
#include "cli/mutex.h"
#include "uout/status_json.hh"
#include "debug/log.h"
#include "jsmn/jsmn.h"
#include "jsmn/jsmn_iterate.hh"
#include <string.h>

#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "cli.cmdline_processing"

bool (*cli_hook_checkPassword)(clpar p[], int len, class UoutWriter &td);
bool (*cli_hook_process_json)(char *json);
bool (*cli_hook_process_json_obj)(class UoutWriter &td,Jsmn_String::Iterator &it);


/////////////////////////////////private/////////////////////////////////////////////////////////
static int handle_parm_json(char *json, jsmntok_t *tok, const char *name);
//static void parse_and_process_json(char *json, class UoutWriter &td, process_parm_cb proc_parm);
//static bool parse_and_process_jsmn(Jsmn_String::Iterator &it, class UoutWriter &td, process_parm_cb proc_parm);
static char* stringFromToken(char *json, const jsmntok_t *tok);

static int handle_parm_json(char *json, jsmntok_t *tok, const char *name) {
  const int oc = tok[0].size;
  ++tok;
  for (int i = 0; i < oc; ++i) {
    if (tok[i].type == JSMN_OBJECT) {
      handle_parm_json(json, &tok[i], stringFromToken(json, &tok[i - 1]));
    } else {

    }
  }
  return 0;
}

/**
 * \brief            Get value of token as string (will null-terminate the string in place)
 * \param    json    the json data which was parsed (will be modified)
 * \param    tok     the parsed token
 * \return           the value of the token as null terminated string (in place)
 */
static char* stringFromToken(char *json, const jsmntok_t *tok) {
  json[tok->end] = '\0';
  return json + tok->start;
}

static bool parse_and_process_jsmn(Jsmn_String::Iterator &it, class UoutWriter &td, process_parm_cb proc_parm) {

  // "json" object is allowed to have nested content
  if (it.keyIsEqual("json", JSMN_OBJECT)) {
    if (cli_hook_process_json_obj) {
      // pass iterator by value copy
      auto it_copy = it;
      cli_hook_process_json_obj(td, ++it_copy);
    }
    // skip the json object
    Jsmn_String::skip_key_and_value(it);
    return true;
  }

  // handle any objects here. The command names of CLI handlers are not know by this component
  // nested objects or arrays are not allowed
  if (it.keyIsEqual(nullptr, JSMN_OBJECT)) {
    int err = 0;
    auto count = it[1].size;
    const int par_len = 20;
    int par_idx = 0;
    clpar par[par_len] = { };
     (par[par_idx].key = it.getValueAsString()) || ++err;;
    par[par_idx++].val = "";

    for (it += 2; count > 0 && it; --count) {
      (par[par_idx].key = it++.getValueAsString()) || ++err;
      (par[par_idx++].val = it++.getValueAsString()) || ++err;
    }

    if (err) {
     db_loge(logtag, "%s: failed no nested objects allowed in cli commands", __func__);
    }
    if (auto res = proc_parm(par, par_idx, td); res < 0) {
      db_loge(logtag, "%s: handler for <%s> returned <%d>", __func__, par[0].key, res);
    }
    return true;
  }

  // eat up any non object tokens (e.g. from":"wapp")
  Jsmn_String::skip_key_and_value(it);
  return true;
}

static bool parse_and_process_json(char *json, class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_json: %s", json));

  auto jsmn = Jsmn_String(json, 128);//Jsmn<128, char *>(json);
  if (!jsmn)
    return false;
  auto it = jsmn.begin();

  int err = 0;
  auto count = it->size;
  for (++it; count > 0 && it; --count) {
     parse_and_process_jsmn(it, td, proc_parm);
  }
  return !err;
}

//////////////////////////////public//////////////////////////////////////////////////////

int cli_processParameters(clpar p[], int len, class UoutWriter &td) {
  assert(len > 0);

  if (!cli_parmHandler_find_cb)
    return -1;

  const char *parm = p[0].key;
  if (auto handler = cli_parmHandler_find_cb(parm)) {
    return handler->process_parmX(p, len, td);
  }

  return -1;
}

void cli_process_json(char *json, class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_json: %s", json));

  if (cli_hook_process_json && cli_hook_process_json(json))
    return;

  if (td.sj().open_root_object("cli")) {
    parse_and_process_json(json, td, proc_parm);
    td.sj().close_root_object();
  }

  if (so_tgt_test(SO_TGT_CLI)) {
    td.sj().writeln_json();
  }
  if (so_tgt_test(SO_TGT_WS)) {
    td.sj().write_json();
  }
}

void cli_process_cmdline(char *line, class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_cmdline: %s", line));
  clpar par[20] = { };
  struct cli_parm clp = { .par = par, .size = 20 };

  int n = cli_parseCommandline(line, &clp);
  if (n < 0) {
    cli_replyFailure(td);
  } else if (n > 0) {

    if (cli_hook_checkPassword && !cli_hook_checkPassword(clp.par, n, td))
      return;

    if (td.so().root_open("cli")) {
      proc_parm(clp.par, n, td);
      td.so().root_close();
#ifdef CONFIG_APP_USE_WS
      if (so_tgt_test(SO_TGT_WS)) {
        uoCb_publish_wsJson(td.sj().get_json());
      }
#endif
    }
  }
}
