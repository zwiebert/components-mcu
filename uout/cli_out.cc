#include "utils_misc/bcd.h"
#include "uout/uo_types.h"
#include "uout/uout_builder_json.hh"
#include "uout/cli_out.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <string.h>
#include <utils_misc/cstring_utils.hh>
#include <stdio.h>

extern uint16_t cli_msgid;



void cli_replySuccess(class UoutWriter &td) {
  reply_message(td, 0, "ok");
}

int cli_replyFailure(class UoutWriter &td) {
  reply_message(td, 0, "error");
  return -1;
}

bool cli_replyResult(class UoutWriter &td, bool success) {
  if (success)
    cli_replySuccess(td);
  else
    cli_replyFailure(td);
  return success;
}

extern int ENR; // error number
void  print_enr(class UoutWriter &td) {
  char buf[64];
  if (int n = snprintf(buf, sizeof buf, "enr: %d\n", ENR); n > 0 && n < sizeof buf) {
    td.write(buf, n);
  }
}

void msg_print(class UoutWriter &td, const char *msg, const char *tag) {
  if (!cli_isInteractive())
    return;
  if (msg)
    td.write(msg);
  if (cli_msgid) {
    char buf[64];
    if (int n = snprintf(buf, sizeof buf, "@%d\n", (int) cli_msgid); n > 0 && n < sizeof buf) {
      td.write(buf, n);
    }
  }
  if (tag) {
    td.write(':');
    td.write(tag);
  }
  td.write(": ");
}

void  cli_warning_optionUnknown(class UoutWriter &td, const char *key) {
 if (!cli_isInteractive())
    return;
  msg_print(td, "warning", "unknown-option"), td.write(key), td.write('\n');
}

void  cli_reply_print(class UoutWriter &td, const char *tag) {
 if (!cli_isInteractive())
    return;
  msg_print(td, "cli_reply", tag);
}

void  reply_message(class UoutWriter &td, const char *tag, const char *msg) {
 if (!cli_isInteractive())
    return;
  cli_reply_print(td, tag);
  if (msg)
    td.write(msg);
  td.write('\n');
}

void  cli_msg_ready(class UoutWriter &td) {
 if (!cli_isInteractive())
    return;
  td.write("\nready:\n");
}

void  reply_id_message(class UoutWriter &td, uint16_t id, const char *tag, const char *msg) {
  uint16_t old_id = cli_msgid;
 if (!cli_isInteractive())
    return;

  cli_msgid = id;
  cli_reply_print(td, tag);
  if (msg)
    td.write(msg);
  td.write('\n');
  cli_msgid = old_id;
}

///////////////////////////////////////////////////////////////////

#define SET_OBJ_TAG(tag) STRLCPY(Obj_tag, (tag), sizeof Obj_tag)
#define OBJ_TAG (Obj_tag+0)

extern uint16_t cli_msgid;

#define td myTd

void  UoutBuilderPlaintext::cli_out_start_reply() {
  char buf[64];
  if (int n = snprintf(buf, sizeof buf, "tf: cli_reply=%d: %s:", (int)cli_msgid, OBJ_TAG); n > 0 && n < sizeof buf) {
    myTd.write(buf, n);
  }
}


const int OUT_MAX_LEN = 80;

void UoutBuilderPlaintext::cli_out_entry(const char *key, const char *val, int len) {
  if (!key || len == -1) {
    if (myLength) {
      myTd << ";\n";
      myLength = 0;
    }
  } else {
    if (key)
      len += strlen(key) + 1;
    if (val)
      len += strlen(val);

    if ((myLength + len) > OUT_MAX_LEN && myLength > 0) {
      myTd.write(";\n");
      myLength = 0;
    }
    if (myLength == 0) {
      cli_out_start_reply();
    }
    myLength += len;
    if (key) {
      myTd << ' ' << key << '=' << (val ? val : "\"\"");
    }
  }
}

void UoutBuilderPlaintext::cli_out_set_x(const char *obj_tag) {
  SET_OBJ_TAG(obj_tag);
}

void UoutBuilderPlaintext::cli_out_x_reply_entry(const char *key, const char *val, int len) {
  if (!cli_isInteractive())
    return;
  cli_out_entry(key, val, len);
}
void UoutBuilderPlaintext::cli_out_close() {
  cli_out_x_reply_entry(0, 0, 0);
}

