/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "cli_private.h"
#include "misc/bcd.h"
#include "cli/cli.h"
#include "uout/status_output.h"
#include "txtio/inout.h"
#include "cli/mutex.hh"
#include "uout/status_json.h"
#include "uout/cli_out.h"
#include "debug/dbg.h"
#include "misc/int_types.h"
#include <string.h>

u16 cli_msgid;

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
    { LockGuard lock(cli_mutex); 
      if (cmdline[0] == '{') {
        sj_write_set(io_write);
        cli_process_json(cmdline, SO_TGT_CLI);
        sj_write_set(0);
      } else {

        io_putlf();
        cli_process_cmdline(cmdline, SO_TGT_CLI);
        cli_msg_ready();
      }
    }
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}


int ENR; // error number
