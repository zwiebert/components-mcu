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


enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun);
void cli_process_cmdline(char *line, so_target_bits tgt);
bool cliBuf_enlarge(struct cli_buf *buf);
int asc2bool(const char *s);
void cli_loop(void);
void cli_setup_task(bool enable);
///////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
