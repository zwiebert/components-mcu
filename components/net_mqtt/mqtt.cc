/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app/config/proj_app_cfg.h"
#ifdef USE_MQTT

#include "net/mqtt/mqtt.h"
#include "net/mqtt/mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.h"
#include "uout/status_json.hh"


bool topic_startsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic, s, s_len));
}

bool topic_endsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic + topic_len - s_len, s, s_len));
}


#endif // USE_MQTT

