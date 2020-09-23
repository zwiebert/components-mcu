#ifdef __cplusplus
  extern "C++" {
#endif
/*
 * spiffs_fs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#pragma once

#ifndef NO_SPIFFS

#include "spiffs_config.h"
#include "spiffs.h"

extern spiffs fs;
#define fs_A (&fs + 0)  // fs_A - our flash file system. sizes is configured in fer_app_config.h

bool spiffs_format_fs(spiffs *fs);  // unmout, format, mount. returns success

#endif /* ENABLE_SPIFFS */
#ifdef __cplusplus
  }
#endif
