/**
 * \file net_mqtt/mqtt.hh
 * \brief C++ wrapper of MQTT client library
 */
#pragma once

#include "stdbool.h"
#include <stdint.h>

struct cfg_mqtt {
  char url[64] = CONFIG_APP_MQTT_URL;
  char user[16] = CONFIG_APP_MQTT_USER;
  char password[31] = CONFIG_APP_MQTT_PASSWORD;
  char client_id[32] = CONFIG_APP_MQTT_CLIENT_ID;
  int8_t enable = CONFIG_APP_MQTT_ENABLE;
};

void io_mqtt_setup(struct cfg_mqtt *cfg_mqt);

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



/// \brief C++ class for wrapping and doing call-backs to MQTT client library
class Net_Mqtt {
public:
  /// \brief              stores object reference of derived class with overriding virtual member functions
  /// \param deribed_obj  pointer to derived object or nullptr.  the object must remain alive until its overwritten by another call
  static void setup(Net_Mqtt *derived_obj);
  /// \brief get registered derived object reference
  static Net_Mqtt& get_this() {
    return *ourDerivedObj;
  }
public:
  virtual ~Net_Mqtt() = default;

public:
  /// \brief event: client has been connected to MQTT server
  virtual void connected() {
  }
  /// \brief event: client hat been disconnected from MQTT server
  virtual void disconnected() {
  }

  /// \brief            event: TOPIC has been subscribed to
  /// \param topic      non null terminated string
  /// \param topic_len  length of TOPIC string
  virtual void subscribed(const char *topic, int topic_len) {
  }

  /// \brief            event: TOPIC has been unsubscribed to
  /// \param topic      non null terminated string
  /// \param topic_len  length of TOPIC string
  virtual void unsubscribed(const char *topic, int topic_len) {
  }

  /// \brief            event: MQTT message has been published
  /// \param msg_id     ID of published message
  virtual void published(int msg_id) {
  }

  /// \brief            event: MQTT message has been received
  /// \param topic      non null terminated TOPIC string
  /// \param topic_len  length of TOPIC string
  /// \param data       non null terminated CONTENT string
  /// \param data_len   length of CONTENT string
  virtual void received(const char *topic, int topic_len, const char *data, int data_len) {
  }

public:
  /// \brief        subscribe to TOPIC
  /// \param topic  null terminated TOPIC string
  /// \param qos    quality of service
  virtual void subscribe(const char *topic, int qos);
  /// \brief        unsubscribe from TOPIC
  /// \param topic  null terminated TOPIC string
  virtual void unsubscribe(const char *topic);
  /// \brief            publish MQTT message
  /// \param topic      null terminated TOPIC string
  /// \param data       null terminated CONTENT string
  /// \param retain     Set retain flag in MQTT message
  virtual void publish(const char *topic, const char *data, bool retain = false);
private:
  static Net_Mqtt *ourDerivedObj;
};
