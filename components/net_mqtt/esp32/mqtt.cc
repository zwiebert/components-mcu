#include "app_config/proj_app_cfg.h"
#include "net_mqtt/mqtt.h"
#include "net_mqtt/mqtt_imp.h"
#include "cli/cli.h"
#include "txtio/inout.h"
#include <uout/uo_callbacks.h>

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
#include <net_mqtt/mqtt.hh>
#include <mbedtls/error.h>

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#ifdef DISTRIBUTION
#define D(x)
#else
#define D(x) x
#endif
#ifdef USE_MQTT

static bool is_connected;
constexpr const char *TAG = "mqtt_client";

// implementation interface

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
  //esp_mqtt_client_handle_t client = event->client;
  //int msg_id;
  // your_context_t *context = event->context;

  switch (event->event_id) {
  case MQTT_EVENT_CONNECTED:
    is_connected = true;
    uoCb_publish_logMessage( { .tag = TAG, .txt = "connected", .warn_level = LogMessage::wl_Info });
    Net_Mqtt::get_this().connected();
    break;

  case MQTT_EVENT_DISCONNECTED:
    uoCb_publish_logMessage( { .tag = TAG, .txt = "disconnected", .warn_level = LogMessage::wl_Info });
    is_connected = false;
    Net_Mqtt::get_this().disconnected();
    break;

  case MQTT_EVENT_SUBSCRIBED: {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "subscribed: msg_id=%d", event->msg_id)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
    Net_Mqtt::get_this().subscribed(event->topic, event->topic_len);
    break;

  case MQTT_EVENT_UNSUBSCRIBED: {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "unsubscribed: msg_id=%d", event->msg_id)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }

    Net_Mqtt::get_this().unsubscribed(event->topic, event->topic_len);
    break;

  case MQTT_EVENT_PUBLISHED: {
    char buf[128];
    if (sizeof buf
        >= snprintf(buf, sizeof buf, "published: msg_id=%d", event->msg_id)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
    Net_Mqtt::get_this().published(event->msg_id);
    break;

  case MQTT_EVENT_DATA: {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "received: TOPIC=<%.*s>, DATA=<%.*s>", event->topic_len, event->topic, event->data_len, event->data)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
    Net_Mqtt::get_this().received(event->topic, event->topic_len, event->data, event->data_len);

    break;
  case MQTT_EVENT_ERROR: {
    const char *txt = "error";
    if (auto eh = event->error_handle) {
      if (eh->error_type == MQTT_ERROR_TYPE_ESP_TLS) {
        txt = "TLS error";
#ifdef MQTT_SUPPORTED_FEATURE_TRANSPORT_ERR_REPORTING
        char buf[128];
        mbedtls_strerror(eh->esp_tls_last_esp_err, buf, sizeof buf);
        if (*buf)
          txt = buf;
        else {
          snprintf(buf, sizeof buf, "TLS error: %x", eh->esp_tls_last_esp_err);
          txt = buf;
        }
#endif
      } else if (eh->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
        switch (eh->connect_return_code) {
        case MQTT_CONNECTION_ACCEPTED:
        break;
        case MQTT_CONNECTION_REFUSE_PROTOCOL:
          txt = "MQTT connection refused reason: Wrong protocol";
          break;
        case MQTT_CONNECTION_REFUSE_ID_REJECTED:
          txt = "MQTT connection refused reason: ID rejected";
          break;
        case MQTT_CONNECTION_REFUSE_SERVER_UNAVAILABLE:
          txt = "MQTT connection refused reason: Server unavailable";
          break;
        case MQTT_CONNECTION_REFUSE_BAD_USERNAME:
          txt = "MQTT connection refused reason: Wrong user";
          break;
        case MQTT_CONNECTION_REFUSE_NOT_AUTHORIZED:
          txt = "MQTT connection refused reason: Wrong username or password";
          break;
        }
      }
    }
    uoCb_publish_logMessage( { .tag = TAG, .txt = txt, .warn_level = LogMessage::wl_Fail });
  }
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

  int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
  {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "subscribe: msg_id=%d, TOPIC=<%s>, QOS=<%d>", msg_id, topic, qos)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
}

void io_mqtt_unsubscribe(const char *topic) {
  if (!client || !is_connected)
    return;

  int msg_id = esp_mqtt_client_unsubscribe(client, topic);
  {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "subscribe: msg_id=%d, TOPIC=<%s>", msg_id, topic)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
}

void io_mqtt_publish(const char *topic, const char *data) {
  if (!client || !is_connected)
    return;

  int msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
  {
    char buf[128];
    if (sizeof buf >= snprintf(buf, sizeof buf, "publish: msg_id=%d, TOPIC=<%s>, DATA=<%s>", msg_id, topic, data)) {
      uoCb_publish_logMessage( { .tag = TAG, .txt = buf, .warn_level = LogMessage::wl_Info });
    }
  }
}

static void io_mqtt_create_client(struct cfg_mqtt *cmc) {
  esp_mqtt_client_config_t mqtt_cfg = { .event_handle = mqtt_event_handler, .uri = cmc->url, .client_id = cmc->client_id,
  //  .host = CONFIG_MQTT_HOST,
  //  .port = CONFIG_MQTT_PORT,
      .username = cmc->user, .password = cmc->password,

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
