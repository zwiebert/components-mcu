#ifdef __cplusplus
extern "C" {
#endif
#pragma once

#include "cli_types.h"
#include "cli/cli.h"
#include "uout/status_output.h"

extern bool (*cli_hook_process_json)(char *json);

void cli_process_json2(char *json, so_target_bits tgt, process_parm_cb proc_parm);
void cli_process_json(char *json, so_target_bits tgt);
char* json_get_command_object(char *s, char **ret_name, char **next);
int parse_json(char *name, char *s, struct cli_parm *clp);
void cli_print_json(const char *json);

#ifdef __cplusplus
}
#endif
