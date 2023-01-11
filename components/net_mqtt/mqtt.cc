#include "net_mqtt/mqtt.hh"

#include "cli/cli.h"
#include "cli/mutex.h"
#include "uout/status_json.hh"

#include <string.h>
#include <stdio.h>



bool topic_startsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic, s, s_len));
}

bool topic_endsWith(const char *topic, int topic_len, const char *s) {
  size_t s_len = strlen(s);
  return topic_len >= s_len && (0 == strncmp(topic + topic_len - s_len, s, s_len));
}

static Net_Mqtt My_base_obj;
Net_Mqtt *Net_Mqtt::ourDerivedObj = &My_base_obj;

void Net_Mqtt::setup(Net_Mqtt *derived_obj) {
  ourDerivedObj = derived_obj ? derived_obj : &My_base_obj;
}


