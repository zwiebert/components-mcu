/*
 * cli_setcmdline.c
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#include "cli/cli.h"
#include "cli_private.h"
#include "txtio/inout.h"
#include <string.h>
#include <stdlib.h>

bool cliBuf_enlarge(struct cli_buf *buf) {
  unsigned new_size = buf->buf_size ? buf->buf_size + 32 : 64;
  void *p = realloc(buf->buf, new_size);
  if (!p)
    return false;

  buf->buf = static_cast<char *>(p);
  buf->buf_size = new_size;
  return true;
}


char* get_commandline() {
  static struct cli_buf buf;

  if (!buf.buf)
    if (!cliBuf_enlarge(&buf))
      return 0;
  for (;;) {
    switch (cli_get_commandline(&buf, io_getc_fun)) {
    case CMDL_DONE:
      return buf.buf;
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

