#ifdef __cplusplus
  extern "C" {
#endif
/*
 * http_client.h
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */

#pragma once

#include <stdbool.h>

bool httpClient_downloadFile(const char *srcUrl, const char *dstFile);
void httpClient_setup(void);

#ifdef __cplusplus
  }
#endif
