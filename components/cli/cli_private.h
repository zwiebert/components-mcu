#pragma once
#include <app_config/proj_app_cfg.h>

#include "cli/cli.h"

// XXX: functions working on a static buffer
char* get_commandline(void);
void cli_process_cmdline2(char *line, const struct TargetDesc &td, process_parm_cb proc_parm);
int cli_processParameters(clpar p[], int len, const struct TargetDesc &td);
int cli_parseCommandline(char *cl, struct cli_parm *clp);

