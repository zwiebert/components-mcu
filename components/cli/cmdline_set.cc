/*
 * cli_setcmdline.c
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#include "cli_private.h"
#include "cli/cli.h"
#include "txtio/inout.h"
#include <string.h>
#include <stdlib.h>

bool cliBuf_enlarge(struct cli_buf *buf) {
  unsigned new_size = buf->size ? buf->size + 32 : 64;
  void *p = realloc(buf->cli_buf, new_size);
  if (!p)
    return false;

  buf->cli_buf = static_cast<char *>(p);
  buf->size = new_size;
  return true;
}


char* get_commandline() {
  static struct cli_buf buf;

  if (!buf.cli_buf)
    if (!cliBuf_enlarge(&buf))
      return 0;
  for (;;) {
    switch (cli_get_commandline(&buf, io_getc_fun)) {
    case CMDL_DONE:
      return buf.cli_buf;
      break;

    case CMDL_INCOMPLETE:
      break;

    case CMDL_LINE_BUF_FULL:
      if (cliBuf_enlarge(&buf))
        continue;
      break;

    case CMDL_ERROR:
      break;
    }

    return 0;
  }
  return 0;
}

