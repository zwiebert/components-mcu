/*
 * cli_setcmdline.c
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#include "cli_app_cfg.h"
#include "cli/cli.h"
#include "txtio/inout.h"
#include <string.h>

char* get_commandline() {
  static char cmd_buf[CMD_BUF_SIZE];
  static int cli_buf_idx, quote_count;

  switch (cli_get_commandline(cmd_buf, CMD_BUF_SIZE, &cli_buf_idx, &quote_count, io_getc_fun)) {
  case CMDL_DONE:
    return cmd_buf;
    break;

  case CMDL_INCOMPLETE:
    break;

  case CMDL_LINE_BUF_FULL:
    cli_buf_idx = quote_count = 0; // TODO: enlarge cli_buf with realloc()
    break;

  case CMDL_ERROR:
    break;
  }

  return 0;
}

