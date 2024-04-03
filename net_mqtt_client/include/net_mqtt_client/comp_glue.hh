/**
 * \brief  optional header only glue inline functions, to glue components together.
 *         This header is supposed  to be included by an application.
 *
 *  To access this file, the caller need to have required modules: uout and config_kvs
 *  
 *
 */

#pragma once


#if __has_include(<config_kvs/settings.hh>) &&  __has_include(<uout/so_target_desc.hh>)
#include <config_kvs/settings.hh>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config_kvs.h>
#include <app_config/options.hh>
#include "net_mqtt_client/mqtt.hh"

#ifdef CONFIG_APP_USE_MQTT
inline struct cfg_mqtt* config_read_mqttClient(struct cfg_mqtt *c) {
  kvshT h;
  if ((h = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ))) {
    kvsRead_charArray(h, CB_MQTT_URL, c->url);
    kvsRead_charArray(h, CB_MQTT_USER, c->user);
    kvsRead_charArray(h, CB_MQTT_PASSWD, c->password);
    kvsRead_charArray(h, CB_MQTT_CLIENT_ID, c->client_id);
    kvsRead_charArray(h, CB_MQTT_ROOT_TOPIC, c->root_topic);
    kvsRead_i8(h, CB_MQTT_ENABLE, c->enable);
    kvs_close(h);
  }
  return c;
}

inline void config_setup_mqttClient(struct cfg_mqtt *cp) {
  struct cfg_mqtt c;
  config_read_mqttClient(&c);
  io_mqtt_setup(&c);
}

#include <uout/so_target_desc.hh>
#include "net_mqtt_client/mqtt.hh"

inline void soCfg_MQTT(class UoutWriter &td, bool backup) {
  cfg_mqtt c;
  config_read_mqttClient(&c);

  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_URL), c.url);
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_USER), c.user);
  if (!backup) td.so().print(comp_sett.get_optKeyStr(CB_MQTT_PASSWD), *c.password ? "*" : "");
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_CLIENT_ID), c.client_id);
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_ROOT_TOPIC), c.root_topic);
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_ENABLE), c.enable);
}

inline void soCfg_MQTT_ENABLE(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_ENABLE), config_read_mqttClient(&c)->enable);
}
inline void soCfg_MQTT_URL(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_URL), config_read_mqttClient(&c)->url);
}
inline void soCfg_MQTT_USER(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_USER), config_read_mqttClient(&c)->user);
}
inline void soCfg_MQTT_PASSWORD(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_PASSWD), *config_read_mqttClient(&c)->password ? "*" : "");
}
inline void soCfg_MQTT_CLIENT_ID(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_CLIENT_ID), config_read_mqttClient(&c)->client_id);
}
inline void soCfg_MQTT_ROOT_TOPIC(class UoutWriter &td) {
  cfg_mqtt c;
  td.so().print(comp_sett.get_optKeyStr(CB_MQTT_ROOT_TOPIC), config_read_mqttClient(&c)->root_topic);
}
#endif

template<typename settings_type>
constexpr void netMqtt_register_settings(settings_type &settings){
#ifdef CONFIG_APP_USE_MQTT
    settings.initField(CB_MQTT_ENABLE, "C_MQTT_ENABLE", otok::k_mqtt_enable, CBT_i8, soCfg_MQTT_ENABLE, STF_direct_bool);
    settings.initField(CB_MQTT_URL, "C_MQTT_URL", otok::k_mqtt_url, CBT_str, soCfg_MQTT_URL, STF_direct);
    settings.initField(CB_MQTT_USER, "C_MQTT_USER", otok::k_mqtt_user, CBT_str, soCfg_MQTT_USER, STF_direct);
    settings.initField(CB_MQTT_PASSWD, "C_MQTT_PASSWD", otok::k_mqtt_password, CBT_str, soCfg_MQTT_PASSWORD, STF_direct);
    settings.initField(CB_MQTT_CLIENT_ID, "C_MQTT_CID", otok::k_mqtt_client_id, CBT_str, soCfg_MQTT_CLIENT_ID, STF_direct);
    settings.initField(CB_MQTT_ROOT_TOPIC, "C_MQTT_RTOPIC", otok::k_mqtt_root_topic, CBT_str, soCfg_MQTT_ROOT_TOPIC, STF_direct);
#endif
}

#endif
