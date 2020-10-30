#pragma once

extern void (*io_mqtt_subscribe_cb)(const char *topic, int qos);
extern void (*io_mqtt_unsubscribe_cb)(const char *topic);
extern void (*io_mqtt_publish_cb)(const char *topic, const char *data);
