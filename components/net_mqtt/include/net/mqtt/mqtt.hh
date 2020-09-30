#pragma once
#include "mqtt.h"

class Net_Mqtt {
public:
  static void setup(Net_Mqtt *derived_obj);
  static Net_Mqtt& get_this() {
    return *ourDerivedObj;
  }
public:
  virtual ~Net_Mqtt() = default;

public:
  virtual void connected() {
    if (io_mqtt_connected_cb)
      io_mqtt_connected_cb();
  }
  virtual void disconnected() {
    if (io_mqtt_disconnected_cb)
      io_mqtt_disconnected_cb();
  }
  virtual void subscribed(const char *topic, int topic_len) {
    if (io_mqtt_subscribed_cb)
      io_mqtt_subscribed_cb(topic, topic_len);
  }
  virtual void unsubscribed(const char *topic, int topic_len) {
    if (io_mqtt_unsubscribed_cb)
      io_mqtt_unsubscribed_cb(topic, topic_len);
  }
  virtual void published(int msg_id) {
    if (io_mqtt_published_cb)
      io_mqtt_published_cb(msg_id);
  }
  virtual void received(const char *topic, int topic_len, const char *data, int data_len) {
    if (io_mqtt_received_cb)
      io_mqtt_received_cb(topic, topic_len, data, data_len);
  }

public:
  static void subscribe(const char *topic, int qos);
  static void unsubscribe(const char *topic);
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
