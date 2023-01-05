/**
 * \file     net_mqtt/mqtt.h
 * \brief    C wrapper around MQTT-client library
 * \note     There is mqtt.hh for a C++ interface
 */

#ifdef __cplusplus
extern "C++" {
#endif

#pragma once


#include "stdbool.h"
#include <stdint.h>

struct cfg_mqtt {
  char url[64];
  char user[16];
  char password[31];
  char client_id[32];
  int8_t enable;
};

void io_mqtt_subscribe(const char *topic, int qos);
void io_mqtt_unsubscribe(const char *topic);
void io_mqtt_publish(const char *topic, const char *data);
void io_mqtt_setup(struct cfg_mqtt *cfg_mqt);

// callbacks (in C++ derive from Net_Mqtt class instead)
extern void (*io_mqtt_connected_cb)(); ///< \brief event callback: MQTT client connected to server
extern void (*io_mqtt_disconnected_cb)(); ///< \brief event callback: MQTT client disconnected from server
extern void (*io_mqtt_subscribed_cb)(const char *topic, int topic_len); ///< \brief event callback: TOPIC  has been subscribed
extern void (*io_mqtt_unsubscribed_cb)(const char *topic, int topic_len); ///< \brief event callback: TOPIC has been unsubscribed
extern void (*io_mqtt_published_cb)(int msg_id); ///< \brief message with MSG_ID has been published

/**
 * \brief             event callback: Message has been received
 * \param topic       non null terminated string containing the topic of the message
 * \param topic_len   topic string length
 * \param data        non null terminated string containing the data of the message
 * \param data_len    data string length
 */
extern void (*io_mqtt_received_cb)(const char *topic, int topic_len, const char *data, int data_len);

/// \brief test if TOPIC with TOPIC_LEN starts with string S
bool topic_startsWith(const char *topic, int topic_len, const char *s);
/// \brief test if TOPIC with TOPIC_LEN ends with string S
bool topic_endsWith(const char *topic, int topic_len, const char *s);

#if !defined CONFIG_APP_USE_MQTT && !defined HOST_TESTING
inline void io_mqtt_subscribe(const char *topic, int qos) {
}
inline void io_mqtt_unsubscribe(const char *topic) {
}
inline void io_mqtt_publish(const char *topic, const char *data) {
}
inline void io_mqtt_setup(struct cfg_mqtt *cfg_mqt) {
}
#endif

#ifdef __cplusplus
}
#endif
