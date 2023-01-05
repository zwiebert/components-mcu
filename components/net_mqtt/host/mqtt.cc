
#include "net_mqtt/mqtt.h"
#include "net_mqtt/mqtt_imp.h"


#include <stddef.h>
#include <string.h>
#include <stdbool.h>

void (*io_mqtt_subscribe_cb)(const char *topic, int qos);
void (*io_mqtt_unsubscribe_cb)(const char *topic);
void (*io_mqtt_publish_cb)(const char *topic, const char *data);

void io_mqtt_subscribe(const char *topic, int qos) {
  if (io_mqtt_subscribe_cb)
    io_mqtt_subscribe_cb(topic, qos);
}


void io_mqtt_unsubscribe(const char *topic) {
  if (io_mqtt_unsubscribe_cb)
    io_mqtt_unsubscribe_cb(topic);
}

void io_mqtt_publish(const char *topic, const char *data) {
  if (io_mqtt_publish_cb)
    io_mqtt_publish_cb(topic, data);
}

