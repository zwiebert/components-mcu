/*
 * http_client.h
 *
 *  Created on: 10.06.2019
 *      Author: bertw
 */

#ifndef MAIN_HTTP_CLIENT_H_
#define MAIN_HTTP_CLIENT_H_

#include <stdbool.h>

bool httpClient_downloadFile(const char *srcUrl, const char *dstFile);


#endif /* MAIN_HTTP_CLIENT_H_ */
