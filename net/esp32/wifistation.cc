
#ifdef CONFIG_APP_USE_WLAN
#include "net/wifi_station_setup.hh"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "string.h"
#include <utils_misc/cstring_utils.hh>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "net/ipnet.h"
#include "txtio/inout.h"
#include "utils_misc/int_types.h"
#include "main_loop/main_queue.hh"

#define printf con_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;
extern ipnet_cb ipnet_gotIpAddr_cb, ipnet_lostIpAddr_cb;


static void
user_set_station_config(struct cfg_wlan *cwl) {

  wifi_config_t sta_config = { };

  if (strlen(cwl->SSID) > (sizeof sta_config.sta.ssid - 1))
    return;
  if (strlen(cwl->password) > (sizeof sta_config.sta.password - 1))
    return;

  STRCPY((char*) sta_config.sta.ssid, cwl->SSID);
  STRCPY((char*) sta_config.sta.password, cwl->password);
  sta_config.sta.bssid_set = false;
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

static int s_retry_num = 0;
const char *TAG = "wifistation";

//#define RETRY_RECONNECT (s_retry_num < 255)
#define RETRY_RECONNECT (1)

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {

  switch (event_id) {

  case WIFI_EVENT_STA_START:
    esp_wifi_connect();
    break;

  case WIFI_EVENT_STA_DISCONNECTED:
    if (RETRY_RECONNECT) {
      ip4_address.addr = 0;
      esp_wifi_connect();
      s_retry_num++;
      // ESP_LOGI(TAG, "retry to connect to the AP");
    }
    break;

  default:
    break;
  }
}

static void lost_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (ipnet_lostIpAddr_cb)
    ipnet_lostIpAddr_cb();

  mainLoop_callFun(ipnet_disconnected);
}

static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void* event_data) {

    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ip4_address = ip_info->ip;
    ip4_gateway_address = ip_info->gw;
    ip4_netmask = ip_info->netmask;

    if (ipnet_gotIpAddr_cb)
      ipnet_gotIpAddr_cb();

    mainLoop_callFun(ipnet_connected);
}

void wifistation_setup(struct cfg_wlan *config)  {
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &lost_ip_event_handler, NULL, NULL));

  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  user_set_station_config(config);
}



#endif //CONFIG_APP_USE_WLAN
