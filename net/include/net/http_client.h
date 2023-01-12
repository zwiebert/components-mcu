/**
 * \file   net/http_client.h
 * \brief  Download file from HTTP server
 */

#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

#include <stdbool.h>

bool httpClient_downloadFile(const char *srcUrl, const char *dstFile);

#ifdef __cplusplus
  }
#endif
