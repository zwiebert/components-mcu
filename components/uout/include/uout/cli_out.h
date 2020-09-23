#pragma once
#include <stdint.h>

void cli_replySuccess(const struct TargetDesc &td);
void cli_warning_optionUnknown(const struct TargetDesc &td, const char *key);
void reply_id_message(const struct TargetDesc &td, uint16_t id, const char *tag, const char *msg);
void reply_message(const struct TargetDesc &td, const char *tag, const char *msg);
bool cli_replyResult(const struct TargetDesc &td, bool success);
int cli_replyFailure(const struct TargetDesc &td);
void print_enr(const struct TargetDesc &td);
void cli_msg_ready(const struct TargetDesc &td);


class StatusTxtT {
  void  cli_out_entry(const char *key, const char *val, int len);
public:
  StatusTxtT(const struct TargetDesc &td): myTd(td) {}
public:
  void cli_out_start_reply();
  void cli_out_set_x(const char *obj_tag);
  void cli_out_close();
  void cli_out_x_reply_entry(const char *key, const char *val, int len = 0);

private:
  int myLength = 0;
  const struct TargetDesc &myTd;
  char Obj_tag[16] = "";
};

