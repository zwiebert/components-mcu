/*
 * http_server.h
 *
 *  Created on: 26.03.2019
 *      Author: bertw
 */

#ifndef SRC_USERIO_HTTP_SERVER_H_
#define SRC_USERIO_HTTP_SERVER_H_


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
//void hts_query_json(char *qstr);


// interface which has to be implemented by mcu specific code
#ifdef USE_HTTP
void hts_enable_http_server(bool enable);
#else
#define hts_enable_http_server(enable)
#endif

void hts_setup(struct cfg_http *config);

#endif /* SRC_USERIO_HTTP_SERVER_H_ */
