/**
 * \file net_mqtt/mqtt.hh
 * \brief C++ wrapper of MQTT client library
 */
#pragma once
#include "mqtt.h"

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
    if (io_mqtt_connected_cb)
      io_mqtt_connected_cb();
  }
  /// \brief event: client hat been disconnected from MQTT server
  virtual void disconnected() {
    if (io_mqtt_disconnected_cb)
      io_mqtt_disconnected_cb();
  }

  /// \brief            event: TOPIC has been subscribed to
  /// \param topic      non null terminated string
  /// \param topic_len  length of TOPIC string
  virtual void subscribed(const char *topic, int topic_len) {
    if (io_mqtt_subscribed_cb)
      io_mqtt_subscribed_cb(topic, topic_len);
  }

  /// \brief            event: TOPIC has been unsubscribed to
  /// \param topic      non null terminated string
  /// \param topic_len  length of TOPIC string
  virtual void unsubscribed(const char *topic, int topic_len) {
    if (io_mqtt_unsubscribed_cb)
      io_mqtt_unsubscribed_cb(topic, topic_len);
  }

  /// \brief            event: MQTT message has been published
  /// \param msg_id     ID of published message
  virtual void published(int msg_id) {
    if (io_mqtt_published_cb)
      io_mqtt_published_cb(msg_id);
  }

  /// \brief            event: MQTT message has been received
  /// \param topic      non null terminated TOPIC string
  /// \param topic_len  length of TOPIC string
  /// \param data       non null terminated CONTENT string
  /// \param data_len   length of CONTENT string
  virtual void received(const char *topic, int topic_len, const char *data, int data_len) {
    if (io_mqtt_received_cb)
      io_mqtt_received_cb(topic, topic_len, data, data_len);
  }

public:
  /// \brief        subscribe to TOPIC
  /// \param topic  null terminated TOPIC string
  /// \param qos    quality of service
  static void subscribe(const char *topic, int qos);
  /// \brief        unsubscribe from TOPIC
  /// \param topic  null terminated TOPIC string
  static void unsubscribe(const char *topic);
  /// \brief            publish MQTT message
  /// \param topic      null terminated TOPIC string
  /// \param data       null terminated CONTENT string
  static void publish(const char *topic, const char *data);
private:
  static Net_Mqtt *ourDerivedObj;
};




//////////////////////////in-line implementation ///////////////////////////////////////
inline void Net_Mqtt::subscribe(const char *topic, int qos) {
  io_mqtt_subscribe(topic, qos);
}
inline void Net_Mqtt::unsubscribe(const char *topic) {
  io_mqtt_unsubscribe(topic);
}
inline void Net_Mqtt::publish(const char *topic, const char *data) {
  io_mqtt_publish(topic,data);
}
