#include "cli_private.h"

#include "txtio/inout.h"
#include "stdbool.h"
#include "cli/cli.h"

typedef int (*getc_funT)(void);



enum cli_get_commline_retT
cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun) {
  bool error = false;
  bool done = false;
  int c;

  if (!((buf->cli_buf_idx + 1) < buf->size)) {
    return CMDL_LINE_BUF_FULL;
  }

  while ((c = (*getc_fun)()) != -1) {

    // line ended before ';' terminator received. throw it away
    if (c == '\r' || c == '\n') {
      buf->quote_count = 0;
      buf->cli_buf_idx = 0;
      continue;
    }

    // backspace: remove last character from buffer
    if (c == '\b') {
      if (buf->cli_buf_idx == 0)
        continue;
      if (buf->cli_buf[--buf->cli_buf_idx] == '\"')
        --buf->quote_count;
      continue;
    }

    // count the quotes, so we know if we are inside or outside a quoted word
    if (c == '\"') {
      ++buf->quote_count;
    }

    // to throw away rest of a too long command line from earlier
    if (error) {
      if (c == ';')
        error = false;
      continue;
    }
    // to make sure there is at least 1 free byte available in CMD_BUF
    if (!((buf->cli_buf_idx + 1) < buf->size)) {
      return CMDL_LINE_BUF_FULL;
    }

    // handle special characters, if outside a quoted word
    if ((buf->quote_count & 1) == 0) {
      // ';' is used to terminate a command line
      if (c == ';' && (buf->quote_count & 1) == 0) {
        if (buf->cli_buf_idx == 0)
          goto succ;
        buf->cli_buf[buf->cli_buf_idx] = '\0';
        goto succ;
      }
    }

    // store char to buffer
    buf->cli_buf[buf->cli_buf_idx] = (char) c;
    ++buf->cli_buf_idx;
  }

  goto cont;
  succ: buf->cli_buf_idx = 0;
  buf->quote_count = 0;
  done = true;
  cont: return error ? CMDL_ERROR : (done ? CMDL_DONE : CMDL_INCOMPLETE);
}
