#ifdef __cplusplus
  extern "C" {
#endif
/*
 * cli_json.h
 *
 *  Created on: 17.03.2020
 *      Author: bertw
 */

#pragma once

#include "cli/cli.h"
#include "userio/status_output.h"

char *json_get_command_object(char *s, char **ret_name, char **next);
int parse_json(char *name, char *s, struct cli_parm *clp);
void cli_print_json(const char *json);

#ifdef __cplusplus
  }
#endif
