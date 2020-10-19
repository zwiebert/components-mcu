#pragma once
#include <net_http_server/http_server_setup.h>

#include "app_config/proj_app_cfg.h"

// interface which has to be implemented by mcu specific code
#ifdef USE_HTTP
void hts_enable_http_server(struct cfg_http *config);
#else
#define hts_enable_http_server(config)
#endif
