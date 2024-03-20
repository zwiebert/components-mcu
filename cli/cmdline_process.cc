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
#include <string.h>

#define D(x)
#define L(x) x
#define logtag "process_cmdline"

bool (*cli_hook_checkPassword)(clpar p[], int len, const class UoutWriter &td);
bool (*cli_hook_process_json)(char *json);

/////////////////////////////////private/////////////////////////////////////////////////////////
static int handle_parm_json(char *json, jsmntok_t *tok, const char *name);
static void parse_and_process_json(char *json, const class UoutWriter &td, process_parm_cb proc_parm);
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
static void parse_and_process_json(char *json, const class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_json: %s", json));

  jsmn_parser jsp;
#define MAX_TOKENS 24
  jsmntok_t tok[MAX_TOKENS];

  jsmn_init(&jsp);
  int nt = jsmn_parse(&jsp, json, strlen(json), tok, MAX_TOKENS);

  int i = 0;

  if (tok[i].type == JSMN_OBJECT) {
    for (i = 1; i < nt; ++i) {

      if (tok[i].type == JSMN_OBJECT) {
        int coi = i; // command object index
        int n_childs = tok[i].size;

        char *cmd_obj = 0;
        if (tok[i - 1].type == JSMN_STRING) {
          cmd_obj = stringFromToken(json, &tok[i - 1]);
        }

        D(db_logi(logtag, "cmd_obj: <%s>", cmd_obj ? cmd_obj : ""));

        if (cmd_obj && strcmp(cmd_obj, "json") == 0) {
          int n = handle_parm_json(json, tok, cmd_obj);
          i += n;
        } else {
          clpar par[20] = { };
          int pi = 0;

          par[pi].key = cmd_obj;
          par[pi++].val = "";

          for (i = coi + 1; i < nt && n_childs > 0; ++i) {
            par[pi].key = stringFromToken(json, &tok[i++]);
            par[pi].val = stringFromToken(json, &tok[i]);
            ++pi;
            --n_childs;
          } D(db_logi(logtag, "proc_parm: %s", par->key));
          proc_parm(par, pi, td);
        }
      }
    }
  }
}

//////////////////////////////public//////////////////////////////////////////////////////

int cli_processParameters(clpar p[], int len, const class UoutWriter &td) {
  assert(len > 0);

  if (!cli_parmHandler_find_cb)
    return -1;

  const char *parm = p[0].key;
  if (auto handler = cli_parmHandler_find_cb(parm)) {
    return handler->process_parmX(p, len, td);
  }

  return -1;
}

void cli_process_json(char *json, const class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_json: %s", json));

  if (cli_hook_process_json && cli_hook_process_json(json))
    return;

  if (td.sj().open_root_object("tfmcu")) {
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

void cli_process_cmdline(char *line, const class UoutWriter &td, process_parm_cb proc_parm) {
  L(db_logi(logtag, "process_cmdline: %s", line));
  clpar par[20] = { };
  struct cli_parm clp = { .par = par, .size = 20 };

  int n = cli_parseCommandline(line, &clp);
  if (n < 0) {
    cli_replyFailure(td);
  } else if (n > 0) {

    if (cli_hook_checkPassword && !cli_hook_checkPassword(clp.par, n, td))
      return;

    if (td.so().root_open("tfmcu")) {
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
