#ifdef __cplusplus
  extern "C++" {
#endif
/*
 * mqtt.h
 *
 *  Created on: 16.03.2019
 *      Author: bertw
 */

#pragma once

#include "app/config/proj_app_cfg.h"
#include "stdbool.h"
#include <stdint.h>

void io_mqtt_enable(bool enable);

struct cfg_mqtt {
char url[64];
char user[16];
char password[31];
char client_id[32];
int8_t enable;
};

#ifdef USE_MQTT
// low level wrapper to hide MQTT implementation
void io_mqtt_subscribe(const char *topic, int qos);
void io_mqtt_unsubscribe(const char *topic);
void io_mqtt_publish(const char *topic, const char *data);
void io_mqtt_setup(struct cfg_mqtt *cfg_mqt);

#else
#define io_mqtt_subscribe(topic, qos)
#define io_mqtt_unsubscribe(topic)
#define io_mqtt_publish(topic, data)
#define setup_mqtt(cfg)
#endif


void io_mqttApp_disconnected();
void io_mqttApp_subscribed(const char *topic, int topic_len);
void io_mqttApp_unsubscribed(const char *topic, int topic_len);
void io_mqttApp_published(int msg_id);



// implementation interface
extern void (*io_mqtt_connected_cb) ();
extern void (*io_mqtt_disconnected_cb) ();
extern void (*io_mqtt_subscribed_cb)(const char *topic, int topic_len);
extern void (*io_mqtt_unsubscribed_cb)(const char *topic, int topic_len);
extern void (*io_mqtt_published_cb)(int msg_id);
extern void (*io_mqtt_received_cb)(const char *topic, int topic_len, const char *data, int data_len);

// helper functions
bool topic_startsWith(const char *topic, int topic_len, const char *s);
bool topic_endsWith(const char *topic, int topic_len, const char *s);



#ifdef __cplusplus
  }
#endif
