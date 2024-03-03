/**
 * \file   net/http_client.h
 * \brief  Download file from HTTP server
 */

#ifdef __cplusplus
  extern "C" {
#endif

#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * \brief    Download data from url to file
 */
bool httpClient_downloadFile(const char *url, const char *file_name);

/**
 * \brief  Get data from url to buffer
 */
bool httpClient_getToBuffer(const char *url, char *buf, size_t buf_size);

#ifdef __cplusplus
  }
#endif
