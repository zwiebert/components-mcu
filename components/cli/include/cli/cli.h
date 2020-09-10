#ifdef __cplusplus
extern "C" {
#endif
#pragma once

#include "uout/status_output.h"
#include "cli_types.h"
#include "cli_json.h"



// external dependencies
extern const struct parm_handlers parm_handlers; // needs to be defined by app

// user API /////////////////////////////////////////////////
extern bool (*cli_hook_checkPassword)(clpar p[], int len, so_target_bits tgt);
extern uint16_t cli_msgid;

void print_enr(void);
enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun);
void cli_process_cmdline(char *line, so_target_bits tgt);
bool cliBuf_enlarge(struct cli_buf *buf);
void cli_replySuccess(void);
void cli_warning_optionUnknown(const char *key);
void reply_id_message(uint16_t id, const char *tag, const char *msg);
void reply_message(const char *tag, const char *msg);
bool cli_replyResult(bool success);
int asc2bool(const char *s);
int cli_replyFailure(void);
void cli_loop(void);
void cli_setup_task(bool enable);
///////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
