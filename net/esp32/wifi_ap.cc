/*
 * wifi_ap.c
 *
 *  Created on: 20.07.2019
 *      Author: bertw
 */
#include <net/wifi_ap_setup.h>

#include "net/ipnet.h"
//#include "net_http_server/http_server_setup.h"
#include "net/tcp_cli_server.h"

#include <string.h>
#include <utils_misc/cstring_utils.hh>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "utils_misc/int_types.h"

#define EXAMPLE_MAX_STA_CONN       4

static const char *TAG = "wifi softAP";
static int connections_since_start;
static int current_connections;
static esp_netif_t *our_wifi;

int wifiAp_get_current_connections() {
 return current_connections;
}

int wifiAp_get_connections_since_ap_start() {
  return connections_since_start;
}
bool wifiAp_is_active() {
  return !!our_wifi;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t*) event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    ++connections_since_start;
    ++current_connections;

  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t*) event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    --current_connections;

  }
}

void wifiAp_setup(const char *ap_ssid, const char *ap_passwd) {
   connections_since_start = current_connections = 0;
  our_wifi = esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

  wifi_config_t wifi_config = { .ap = { .ssid_len = static_cast<uint8_t>(strlen(ap_ssid)), .authmode = WIFI_AUTH_WPA_WPA2_PSK, .max_connection =
      EXAMPLE_MAX_STA_CONN,

  }, };
  STRLCPY((char* )wifi_config.ap.ssid, ap_ssid, sizeof wifi_config.ap.ssid);
  STRLCPY((char* )wifi_config.ap.password, ap_passwd, sizeof wifi_config.ap.password);

  if (strlen(ap_passwd) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_softap finished. ap_ssid:%s password:%s", ap_ssid, ap_passwd);

}

void wifiAp_setdown() {
  esp_netif_destroy_default_wifi(our_wifi);
  current_connections = 0;
  our_wifi = 0;
}

