/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include <cli/cli.h>
#include <cli/cli_types.h>
#include "cli_private.h"
#include "utils_misc/bcd.h"
#include "uout/uo_types.h"
#include "txtio/inout.h"
#include "cli/mutex.hh"
#include "uout/uout_builder_json.hh"
#include "uout/cli_out.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <string.h>
#include <debug/log.h>
#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "cli"

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

#ifdef CONFIG_CLI_INTERACTIVE_CONSOLE
void cli_loop(void) {

  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
  D(db_logi(logtag, "%s() got commandline: <%s>", __func__, cmdline));
    {
      LockGuard lock(cli_mutex);
      if (cmdline[0] == '{') {
         UoutWriterConsole td { SO_TGT_CLI | SO_TGT_FLAG_JSON };
        cli_process_json(cmdline, td);
      } else {
         UoutWriterConsole td { (SO_TGT_CLI | SO_TGT_FLAG_TXT) };
        io_putlf();
        cli_process_cmdline(cmdline, td);
        cli_msg_ready(td);
      }
    }
  } else if (!ready) {
    UoutWriterConsole td { (SO_TGT_CLI | SO_TGT_FLAG_TXT)};
    cli_msg_ready(td);
    ready = true;
  }
}
#else
void cli_loop(void) {

  char *cmdline;
  if ((cmdline = get_commandline())) {
    D(db_logi(logtag, "%s() got commandline: <%s>", __func__, cmdline));
    {
      LockGuard lock(cli_mutex);
      if (cmdline[0] == '{') {
        UoutWriterConsole td { SO_TGT_STM32 | SO_TGT_FLAG_JSON };
        cli_process_json(cmdline, td);
      } else {
      }
    }
  }
}
#endif

int ENR; // error number
