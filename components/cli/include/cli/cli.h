#pragma once

#include "uout/status_output.h"
#include "cli_types.h"



// external dependencies
extern const struct parm_handlers *cli_parm_handlers; // needs to be set by app
extern const parm_handler* (*cli_parmHandler_find_cb)(const char *key);

// user API /////////////////////////////////////////////////
extern bool (*cli_hook_checkPassword)(clpar p[], int len, const struct TargetDesc &td);
int cli_processParameters(clpar p[], int len, const struct TargetDesc &td);
extern uint16_t cli_msgid;


enum cli_get_commline_retT cli_get_commandline(struct cli_buf *buf, getc_funT getc_fun);
void cli_process_cmdline(char *line, const struct TargetDesc &td, process_parm_cb proc_parm = cli_processParameters);
bool cliBuf_enlarge(struct cli_buf *buf);
int asc2bool(const char *s);
void cli_loop(void);
void cli_setup_task(bool enable);
///////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error
#endif
