#include "app_config/proj_app_cfg.h"
#include "mqtt.h"
#include "mqtt_imp.h"
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

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#ifdef DISTRIBUTION
#define D(x)
#else
#define D(x) x
#endif


static const char *TAG = "MQTT_EXAMPLE";

#define CONFIG_MQTT_CLIENT_ID io_mqtt_client_id
const char *io_mqtt_client_id;

static bool is_connected;
static struct cfg_mqtt *io_mqtt_config;
#define cmc io_mqtt_config
// implementation interface

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
  //esp_mqtt_client_handle_t client = event->client;
  //int msg_id;
  // your_context_t *context = event->context;

  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED:
    is_connected = true;
    D(ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED"));
    io_mqtt_connected();
    break;
    
  case MQTT_EVENT_DISCONNECTED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED"));
    is_connected = false;
    io_mqtt_disconnected();
    break;

  case MQTT_EVENT_SUBSCRIBED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id));
    io_mqtt_subscribed(event->topic, event->topic_len);
    break;

  case MQTT_EVENT_UNSUBSCRIBED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id));
    io_mqtt_unsubscribed(event->topic, event->topic_len);
    
    break;
  case MQTT_EVENT_PUBLISHED:
    D(ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id));
    io_mqtt_published(event->msg_id);
    break;
    
  case MQTT_EVENT_DATA:
    D(ESP_LOGI(TAG, "MQTT_EVENT_DATA"));
    D(printf("TOPIC=%.*s\r\n", event->topic_len, event->topic));
    D(printf("DATA=%.*s\r\n", event->data_len, event->data));
    
    io_mqtt_received(event->topic, event->topic_len, event->data, event->data_len);
 
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
  if (!so_tgt_test(SO_TGT_MQTT))
    return;

  D(ESP_LOGI(TAG, "MQTT_PUBLISH, topic=%s, data=%s", topic, data));
  /*int msg_id = */ esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}

static void io_mqtt_create_client(void) {
  esp_mqtt_client_config_t mqtt_cfg = {
      .uri = cmc->url,
  //  .host = CONFIG_MQTT_HOST,
  //  .port = CONFIG_MQTT_PORT,
      .event_handle = mqtt_event_handler,
      .username = cmc->user,
      .password = cmc->password,
      .client_id = CONFIG_MQTT_CLIENT_ID,
  // .user_context = (void *)your_context
      };

    client = esp_mqtt_client_init(&mqtt_cfg);

}

void io_mqtt_create_and_start(void) {
  if (client)
    io_mqtt_stop_and_destroy();
  io_mqtt_create_client();
  esp_mqtt_client_start(client);
}

void io_mqtt_stop_and_destroy(void) {
  if (client) {
    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
    client = 0;
    is_connected = false;
  }
}

void io_mqtt_setup(const char *client_id, struct cfg_mqtt *cfg_mqt) {
  bool is_initialized = !io_mqtt_config;

  if (cfg_mqt)
    io_mqtt_config = cfg_mqt;

  if (!is_initialized) {
    io_mqtt_config = cfg_mqt;
    io_mqtt_client_id = client_id;
    if (TXTIO_IS_VERBOSE(6)) {
      ets_printf("\n\n----#####################################----------\n\n");
      ESP_LOGI(TAG, "[APP] Startup..");
      ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
      ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

      esp_log_level_set("*", ESP_LOG_INFO);
      esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
      esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
      esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
      esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
      esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
    }
  }

  if (io_mqtt_config) {
    io_mqtt_enable(io_mqtt_config->enable);
  }
}
