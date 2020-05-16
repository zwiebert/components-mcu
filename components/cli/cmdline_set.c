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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

char cmd_buf[CMD_BUF_SIZE];

static int blocking_getchar() {
#if 0
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);
  select(STDIN_FILENO+1, &rfds, NULL, NULL, NULL);
#elif 0
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, (flags & ~O_NONBLOCK));
#endif

  char c;
  if (read(STDIN_FILENO, &c, 1) == 1)
    return c;

  return -1;
}

char* get_commandline() {
#ifdef USE_CLI_TASK
  return cli_get_commandline(cmd_buf, CMD_BUF_SIZE, blocking_getchar);
#else
  return cli_get_commandline(cmd_buf, CMD_BUF_SIZE, io_getc_fun);
#endif
}

char*
set_commandline(const char *src, u8 len) {  // FIXME: make sure cmd_buf is not in use by get_commandline()
  if (len >= CMD_BUF_SIZE)
    return 0;

  memcpy(cmd_buf, src, len);
  cmd_buf[len] = '\0';
  return cmd_buf;
}
