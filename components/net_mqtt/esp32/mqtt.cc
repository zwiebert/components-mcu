#include "app/config/proj_app_cfg.h"
#include "net/mqtt/mqtt.h"
#include "net/mqtt/mqtt_imp.h"
#include "cli/cli.h"
#include "txtio/inout.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include <net/mqtt/mqtt.hh>

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#ifdef DISTRIBUTION
#define D(x)
#else
#define D(x) x
#endif
#ifdef USE_MQTT

static const char *TAG = "MQTT_EXAMPLE";
static bool is_connected;

// implementation interface

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
  //esp_mqtt_client_handle_t client = event->client;
  //int msg_id;
  // your_context_t *context = event->context;

  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED:
    is_connected = true;
    D(ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED"));
    Net_Mqtt::get_this().connected();
    break;

  case MQTT_EVENT_DISCONNECTED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED"));
    is_connected = false;
    Net_Mqtt::get_this().disconnected();
    break;

  case MQTT_EVENT_SUBSCRIBED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id));
    Net_Mqtt::get_this().subscribed(event->topic, event->topic_len);
    break;

  case MQTT_EVENT_UNSUBSCRIBED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id));

    Net_Mqtt::get_this().unsubscribed(event->topic, event->topic_len);
    break;
  case MQTT_EVENT_PUBLISHED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id));
    Net_Mqtt::get_this().published(event->msg_id);
    break;

  case MQTT_EVENT_DATA:
    D(ESP_LOGI(TAG, "MQTT_EVENT_DATA"));
    D(printf("TOPIC=%.*s\r\n", event->topic_len, event->topic));
    D(printf("DATA=%.*s\r\n", event->data_len, event->data));
    Net_Mqtt::get_this().received(event->topic, event->topic_len, event->data, event->data_len);
 
    break;
  case MQTT_EVENT_ERROR:
    D(ESP_LOGI(TAG, "MQTT_EVENT_ERROR"));
    break;
  default:
    D(ESP_LOGI(TAG, "Other event id:%d", event->event_id));
    break;
  }
  return ESP_OK;
}

static esp_mqtt_client_handle_t client;


void io_mqtt_subscribe(const char *topic, int qos) {
  if (!client || !is_connected)
    return;

  /*int msg_id = */ esp_mqtt_client_subscribe(client, topic, qos);
}

void io_mqtt_unsubscribe(const char *topic) {
  if (!client || !is_connected)
    return;

  /*int msg_id = */ esp_mqtt_client_unsubscribe(client, topic);
}

void io_mqtt_publish(const char *topic, const char *data) {
  if (!client || !is_connected)
    return;

  D(ESP_LOGI(TAG, "MQTT_PUBLISH, topic=%s, data=%s", topic, data));
  /*int msg_id = */ esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

static void io_mqtt_create_client(struct cfg_mqtt *cmc) {
  esp_mqtt_client_config_t mqtt_cfg = {
      .event_handle = mqtt_event_handler,
      .uri = cmc->url,
      .client_id = cmc->client_id,
  //  .host = CONFIG_MQTT_HOST,
  //  .port = CONFIG_MQTT_PORT,
      .username = cmc->user,
      .password = cmc->password,

  // .user_context = (void *)your_context
      };

  if (mqtt_cfg.uri && *mqtt_cfg.uri == '\0') {
    io_puts("error: MQTT-URI is configured empty\n");
    return;
  }

  client = esp_mqtt_client_init(&mqtt_cfg);

}

static void io_mqtt_stop_and_destroy(void) {
  if (client) {
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
    client = 0;
    is_connected = false;
  }
}

static void io_mqtt_create_and_start(struct cfg_mqtt *c) {
  if (client)
    io_mqtt_stop_and_destroy();
  io_mqtt_create_client(c);
  esp_mqtt_client_start(client);
}


void io_mqtt_setup(struct cfg_mqtt *c) {
    if (c && c->enable) {
      io_mqtt_create_and_start(c);
    } else {
      io_mqtt_stop_and_destroy();
    }
}
#endif
