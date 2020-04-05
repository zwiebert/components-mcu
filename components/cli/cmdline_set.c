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

char cmd_buf[CMD_BUF_SIZE];

char* get_commandline() {
  return cli_get_commandline(cmd_buf, CMD_BUF_SIZE, io_getc_fun);
}

char*
set_commandline(const char *src, u8 len) {  // FIXME: make sure cmd_buf is not in use by get_commandline()
  if (len >= CMD_BUF_SIZE)
    return 0;

  memcpy(cmd_buf, src, len);
  cmd_buf[len] = '\0';
  return cmd_buf;
}
