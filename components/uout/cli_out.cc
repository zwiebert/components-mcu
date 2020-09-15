#include "misc/bcd.h"
#include "uout/status_output.h"
#include "txtio/inout.h"
#include "uout/status_json.h"
#include "uout/cli_out.h"
#include "debug/dbg.h"
#include "misc/int_types.h"
#include <string.h>

extern u16 cli_msgid;

void cli_replySuccess() {
  reply_message(0, "ok");
}

int cli_replyFailure() {
  reply_message(0, "error");
  return -1;
}

bool cli_replyResult(bool success) {
  if (success)
    cli_replySuccess();
  else
    cli_replyFailure();
  return success;
}

extern int ENR; // error number
void  print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void  msg_print(const char *msg, const char *tag) {
 if (!cli_isInteractive())
    return;
  if (msg)
    io_puts(msg);
  if (cli_msgid) {
    io_putc('@');
    io_putd(cli_msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void  cli_warning_optionUnknown(const char *key) {
 if (!cli_isInteractive())
    return;
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void  cli_reply_print(const char *tag) {
 if (!cli_isInteractive())
    return;
  msg_print("cli_reply", tag);
}

void  reply_message(const char *tag, const char *msg) {
 if (!cli_isInteractive())
    return;
  cli_reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void  cli_msg_ready(void) {
 if (!cli_isInteractive())
    return;
  io_puts("\nready:\n");
}

void  reply_id_message(u16 id, const char *tag, const char *msg) {
  u16 old_id = cli_msgid;
 if (!cli_isInteractive())
    return;

  cli_msgid = id;
  cli_reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
  cli_msgid = old_id;
}

void
cli_print_json(const char *json) {
    io_puts(json), io_putlf();
}
