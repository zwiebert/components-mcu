#include "app_config/proj_app_cfg.h"

#include "net_mqtt/mqtt.h"
#include "net_mqtt/mqtt_imp.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.h"
#include "uout/status_json.hh"
#include <net_mqtt/mqtt.hh>

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

void (*io_mqtt_connected_cb)();
void (*io_mqtt_disconnected_cb)();
void (*io_mqtt_subscribed_cb)(const char *topic, int topic_len);
void (*io_mqtt_unsubscribed_cb)(const char *topic, int topic_len);
void (*io_mqtt_published_cb)(int msg_id);
void (*io_mqtt_received_cb)(const char *topic, int topic_len, const char *data, int data_len);

