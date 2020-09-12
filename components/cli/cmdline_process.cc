/*
 * cmdline_process_cli.c
 *
 *  Created on: 18.03.2020
 *      Author: bertw
 */

#include "cli_private.h"
#include "misc/bcd.h"
#include "cli/cli.h"
#include "cli/cli_json.h"
#include "uout/status_output.h"
#include "uout/callbacks.h"
#include "net/http/server/http_server.h"
#include "txtio/inout.h"
#include "cli/mutex.h"
#include "uout/status_json.h"
#include "debug/dbg.h"
#include "jsmn/jsmn.h"
#include <string.h>

#define D(x)

bool (*cli_hook_checkPassword)(clpar p[], int len, so_target_bits tgt);


int cli_processParameters(clpar p[], int len) {
  int i;
  int result = -1;
  precond (len > 0);


  for (i = 0; i < parm_handlers.count; ++i) {
    if (strcmp(p[0].key, parm_handlers.handlers[i].parm) == 0) {
      result = parm_handlers.handlers[i].process_parmX(p, len);
      break;
    }
  }
  return result;
}

bool (*cli_hook_process_json)(char *json);

void cli_process_json(char *json, so_target_bits tgt) {
  if (cli_hook_process_json && cli_hook_process_json(json))
    return;
  cli_process_json2(json, tgt, cli_processParameters);
}

static void cli_process_json3(char *json, process_parm_cb proc_parm);

void cli_process_json2(char *json, so_target_bits tgt, process_parm_cb proc_parm) {
  so_tgt_set(tgt|SO_TGT_FLAG_JSON);
  dbg_vpf(db_printf("process_json: %s\n", json));


  if (sj_open_root_object("tfmcu")) {
    cli_process_json3(json, proc_parm);
    sj_close_root_object();
  }

  if (so_tgt_test(SO_TGT_CLI) && !sj_write) {
    cli_print_json(sj_get_json());
  }

#ifdef USE_WS
  if (so_tgt_test(SO_TGT_WS)) {
    uoApp_publish_wsJson(sj_get_json());
  }
#endif
  so_tgt_default();
}

static char *stringFromToken(char *json, const jsmntok_t *tok) {
  json[tok->end] = '\0';
  return json + tok->start;
}


int handle_parm_json(char *json, jsmntok_t *tok, const char *name) {
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



static void cli_process_json3(char *json, process_parm_cb proc_parm) {
  dbg_vpf(db_printf("process_json: %s\n", json));

  jsmn_parser jsp;
#define MAX_TOKENS 24
  jsmntok_t tok[MAX_TOKENS];

  jsmn_init(&jsp);
  int nt = jsmn_parse(&jsp, json, strlen(json), tok, MAX_TOKENS);

  int i = 0;

  if (tok[i].type == JSMN_OBJECT) {
    int roi = 0; // root object index

    for (i = 1; i < nt; ++i) {

      if (tok[i].type == JSMN_OBJECT) {
        int coi = i; // command object index
        int n_childs = tok[i].size;

        char *cmd_obj = "";
        if (tok[i - 1].type == JSMN_STRING) {
          cmd_obj = stringFromToken(json, &tok[i - 1]);
        }

        D(db_printf("cmd_obj: %s\n", cmd_obj));

        if (strcmp(cmd_obj, "json") == 0) {
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
          }
          D(db_printf("proc_parm: %s\n", par->key));
          proc_parm(par, pi);
        }
      }
    }
  }
}

void cli_process_cmdline(char *line, so_target_bits tgt) {
  cli_process_cmdline2(line, tgt, cli_processParameters);
}

void cli_process_cmdline2(char *line, so_target_bits tgt, process_parm_cb proc_parm) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  so_tgt_set(tgt | SO_TGT_FLAG_TXT);
  clpar par[20] = { };
  struct cli_parm clp = { .par = par, .size = 20 };

  int n = cli_parseCommandline(line, &clp);
  if (n < 0) {
    cli_replyFailure();
  } else if (n > 0) {

    if (cli_hook_checkPassword && !cli_hook_checkPassword(clp.par, n, tgt))
      return;

    if (sj_open_root_object("tfmcu")) {
      proc_parm(clp.par, n);
      sj_close_root_object();
#ifdef USE_WS
      if (so_tgt_test(SO_TGT_WS)) {
        uoApp_publish_wsJson(sj_get_json());
      }
#endif
    }
  }
  so_tgt_default();
}
