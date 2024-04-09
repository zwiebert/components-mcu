#include "cli_private.h"
#include "txtio/inout.h"
#include "cli/cli.h"
#include <debug/log.h>
#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define DD(x) x
#else
#define D(x)
#define DD(x)
#endif
#define logtag "cli"

typedef int (*getc_funT)(void);

enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun) {
  bool error = false;
  int c;

  if (!((buf->buf_idx + 1) < buf->buf_size)) {
    return CMDL_LINE_BUF_FULL;
  }

  while ((c = (*getc_fun)()) != -1) {
    DD((fputc(c, stderr)));
  //  DD((fputc('<', stderr), fputc(c, stderr), fputc('>', stderr)));
   // DD(fprintf(stderr, "%d:%c:%d,", buf->buf_idx, c, c));
#ifndef CONFIG_CLI_CMDLINE_ALLOW_LF
    // line ended before ';' terminator received. throw it away
    if (c == '\r' || c == '\n') {
      buf->quote_count = 0;
      buf->buf_idx = 0;
      goto loop;
    }
#endif

    // backspace: remove last character from buffer
    if (c == '\b') {
      if (buf->buf_idx == 0)
        goto loop;
      if (buf->buf[--buf->buf_idx] == '\"')
        --buf->quote_count;
      goto loop;
    }

    // count the quotes, so we know if we are inside or outside a quoted word
    if (c == '\"') {
      ++buf->quote_count;
    }

    // to throw away rest of a too long command line from earlier
    if (error) {
      if ((c == ';'
#ifdef CONFIG_CLI_CMDLINE_ALLOW_LF
          || c == '\r'
          || c == '\n'
#endif
      ))
        D(db_logw(logtag, "%s: terminator while in error state", __func__));
      error = false;
      goto loop;
    }

    // handle special characters, if outside a quoted word
    if ((buf->quote_count & 1) == 0) {
      // ';' is used to terminate a command line
      if ((c == ';'
#ifdef CONFIG_CLI_CMDLINE_ALLOW_LF
          || c == '\r'
          || c == '\n'
#endif
      )) {
        if ((buf->quote_count & 1) == 0) {
          buf->buf[buf->buf_idx] = '\0';
          D(db_logw(logtag, "%s: got line: <%s>", __func__, buf->buf));
          buf->buf_idx = 0;
          buf->quote_count = 0;
          return error ? CMDL_ERROR : CMDL_DONE;
        } else {
          D(db_logw(logtag, "%s: terminator inside quote", __func__));
        }
      }
    }

    // store char to buffer
    buf->buf[buf->buf_idx] = (char) c;
    ++buf->buf_idx;

    loop: // continue loop
    // to make sure there is at least 1 free byte available in CMD_BUF
    if (!((buf->buf_idx + 1) < buf->buf_size)) {
      return CMDL_LINE_BUF_FULL;
    }
  }

  return error ? CMDL_ERROR : CMDL_INCOMPLETE;
}
