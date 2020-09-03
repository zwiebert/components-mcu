#ifdef __cplusplus
  extern "C" {
#endif
/*
 * http_server.h
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */

#pragma once


#include "app_config/proj_app_cfg.h"
#include "stdbool.h"
#include <stdint.h>

struct cfg_http {
  char user[16];
  char password[31];
  int8_t enable;
};

typedef enum { HQT_NONE, } hts_query_t;

void hts_query(hts_query_t qtype, const char *qstr, int qstr_len);
void hts_query0(hts_query_t qtype, char *qstr);
//void hts_query_json(char *qstr);

struct httpd_req;
bool check_access_allowed(struct httpd_req *req); // XXX: esp32 specific types should be moved to esp32 folder

// interface which has to be implemented by mcu specific code
#ifdef USE_HTTP
void hts_enable_http_server(struct cfg_http *config);
#else
#define hts_enable_http_server(config)
#endif

void hts_setup(struct cfg_http *config);

#ifdef __cplusplus
  }
#endif
