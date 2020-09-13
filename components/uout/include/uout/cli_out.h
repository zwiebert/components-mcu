#pragma once

void cli_replySuccess(void);
void cli_warning_optionUnknown(const char *key);
void reply_id_message(uint16_t id, const char *tag, const char *msg);
void reply_message(const char *tag, const char *msg);
bool cli_replyResult(bool success);
int cli_replyFailure(void);
void print_enr(void);
void cli_print_json(const char *json);
void cli_msg_ready(void);
