/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "cli_private.h"
#include "utils_misc/bcd.h"
#include "cli/cli.h"
#include "uout/uo_types.h"
#include "txtio/inout.h"
#include "cli/mutex.hh"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <string.h>

uint16_t cli_msgid;
const struct parm_handlers *cli_parm_handlers;
const parm_handler* (*cli_parmHandler_find_cb)(const char *key);

int asc2bool(const char *s) {
  if (!s)
    return 1; // default value for key without value

  switch (*s) {
  case '0':
    return 0;
  case '1':
    return 1;
  default:
    return -1;
  }

}

void cli_loop(void) {

  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    {
      LockGuard lock(cli_mutex);
      if (cmdline[0] == '{') {
         TargetDescCon td { SO_TGT_CLI | SO_TGT_FLAG_JSON };
        cli_process_json(cmdline, td);
      } else {
         TargetDescCon td { (SO_TGT_CLI | SO_TGT_FLAG_TXT) };
        io_putlf();
        cli_process_cmdline(cmdline, td);
        cli_msg_ready(td);
      }
    }
  } else if (!ready) {
    const TargetDescCon td { (SO_TGT_CLI | SO_TGT_FLAG_TXT)};
    cli_msg_ready(td);
    ready = true;
  }
}


int ENR; // error number
