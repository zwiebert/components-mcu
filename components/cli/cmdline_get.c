#include "cli_app_cfg.h"

#include "txtio/inout.h"
#include "stdbool.h"
#include "cli/cli.h"

typedef int (*getc_funT)(void);



enum cli_get_commline_retT
cli_get_commandline(char *cbuf, unsigned buf_size, int *cmd_buf_idx, int *quoteCount, getc_funT getc_fun) {
  bool error = false;
  bool done = false;
  int c;

  if (!((*cmd_buf_idx + 1) < buf_size)) {
    return CMDL_LINE_BUF_FULL;
  }

  while ((c = (*getc_fun)()) != -1) {

    // line ended before ';' terminator received. throw it away
    if (c == '\r' || c == '\n') {
      *quoteCount = 0;
      *cmd_buf_idx = 0;
      continue;
    }

    // backspace: remove last character from buffer
    if (c == '\b') {
      if (*cmd_buf_idx == 0)
        continue;
      if (cbuf[--*cmd_buf_idx] == '\"')
        --*quoteCount;
      continue;
    }

    // count the quotes, so we know if we are inside or outside a quoted word
    if (c == '\"') {
      ++*quoteCount;
    }

    // to throw away rest of a too long command line from earlier
    if (error) {
      if (c == ';')
        error = false;
      continue;
    }
    // to make sure there is at least 1 free byte available in CMD_BUF
    if (!((*cmd_buf_idx + 1) < buf_size)) {
      return CMDL_LINE_BUF_FULL;
    }

    // handle special characters, if outside a quoted word
    if ((*quoteCount & 1) == 0) {
      // ';' is used to terminate a command line
      if (c == ';' && (*quoteCount & 1) == 0) {
        if (*cmd_buf_idx == 0)
          goto succ;
        cbuf[*cmd_buf_idx] = '\0';
        goto succ;
      }
    }

    // store char to buffer
    cbuf[*cmd_buf_idx] = (char) c;
    ++*cmd_buf_idx;
  }

  goto cont;
  succ: *cmd_buf_idx = 0;
  *quoteCount = 0;
  done = true;
  cont: return error ? CMDL_ERROR : (done ? CMDL_DONE : CMDL_INCOMPLETE);
}
