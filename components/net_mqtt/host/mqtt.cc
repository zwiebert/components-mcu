
#include "net_mqtt/mqtt.hh"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

void (*io_mqtt_subscribe_cb)(const char *topic, int qos);
void (*io_mqtt_unsubscribe_cb)(const char *topic);
void (*io_mqtt_publish_cb)(const char *topic, const char *data);

void Net_Mqtt::subscribe(const char *topic, int qos) {
  if (io_mqtt_subscribe_cb)
    io_mqtt_subscribe_cb(topic, qos);
}


void Net_Mqtt::unsubscribe(const char *topic) {
  if (io_mqtt_unsubscribe_cb)
    io_mqtt_unsubscribe_cb(topic);
}

void Net_Mqtt::publish(const char *topic, const char *data) {
  if (io_mqtt_publish_cb)
    io_mqtt_publish_cb(topic, data);
}

void io_mqtt_setup(struct cfg_mqtt *cfg_mqt) {
}

