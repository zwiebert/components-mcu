#pragma once
#include <app/config/proj_app_cfg.h>

#include "cli/cli.h"

#ifdef __cplusplus
  extern "C" {
#endif

  // XXX: functions working on a static buffer
  char *get_commandline(void);
  void cli_process_cmdline2(char *line, so_target_bits tgt, process_parm_cb proc_parm);
  int cli_processParameters(clpar p[], int len);
  int cli_parseCommandline(char *cl, struct cli_parm *clp);
#ifdef __cplusplus
  }
#endif


#ifdef __cplusplus
#endif
