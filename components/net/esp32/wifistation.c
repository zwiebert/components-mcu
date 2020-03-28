#include "app_config/proj_app_cfg.h"
#ifdef USE_WLAN
#include "net/wifistation.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "string.h"

#include "esp32/rom/uart.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "net/ipnet.h"
#include "txtio/inout.h"
#include "config/config.h"

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;
extern ipnet_cb ipnet_gotIpAddr_cb, ipnet_lostIpAddr_cb;

void
user_set_station_config(void) {

  wifi_config_t sta_config = { };

  strncpy((char*) sta_config.sta.ssid, C.wifi_SSID, sizeof sta_config.sta.ssid - 1);
  strncpy((char*) sta_config.sta.password, C.wifi_password, sizeof sta_config.sta.password - 1);
  sta_config.sta.bssid_set = false;
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

static int s_retry_num = 0;
const char *TAG = "wifistation";

//#define RETRY_RECONNECT (s_retry_num < 255)
#define RETRY_RECONNECT (1)

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
        i32 event_id, void* event_data) {

    switch (event_id) {

    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "wifi sta start");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "wifi sta disconnected");

        {
            if (RETRY_RECONNECT) {
                ip4_address.addr = 0;
                esp_wifi_connect();
                s_retry_num++;
                // ESP_LOGI(TAG, "retry to connect to the AP");
            }
            break;
        }

    default:
        break;
    }
}

static void lost_ip_event_handler(void *arg, esp_event_base_t event_base, i32 event_id, void *event_data) {
  ESP_LOGI(TAG, "STA Lost IP Address");
  if (ipnet_lostIpAddr_cb)
    ipnet_lostIpAddr_cb();
}

static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
        i32 event_id, void* event_data) {

    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "STA Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "NETMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    ip4_address = ip_info->ip;
    ip4_gateway_address = ip_info->gw;
    ip4_netmask = ip_info->netmask;

    if (ipnet_gotIpAddr_cb)
      ipnet_gotIpAddr_cb();
}

void
wifistation_setup(void) {
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &lost_ip_event_handler, NULL, NULL));

  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  user_set_station_config();
}



#endif //USE_WLAN
