#pragma once

#include "uout/status_output.h"

void cli_out_set_x(const struct TargetDesc &td, const char *obj_tag);
void cli_out_close(const struct TargetDesc &td);
void cli_out_x_reply_entry(const struct TargetDesc &td, const char *key, const char *val, int len = 0);
