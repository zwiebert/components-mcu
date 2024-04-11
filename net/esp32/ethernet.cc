
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp32/rom/gpio.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "debug/dbg.h"
#include "net/ipnet.h"
#include <net/ethernet_setup.hh>
#include "utils_misc/int_types.h"
#include "main_loop/main_queue.hh"
#include "debug/log.h"

#ifdef CONFIG_NETWORK_DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "net"

extern esp_ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

extern "C" esp_eth_phy_t* my_esp_eth_phy_new_lan8720(const eth_phy_config_t *config);
static esp_eth_phy_t* (*ethernet_create_phy)(const eth_phy_config_t *config);



#define DX(x) x

static int8_t ethernet_phy_power_pin = -1;
static esp_netif_t *eth_netif;
static esp_eth_mac_t *mac;
static esp_eth_phy_t *phy;
static esp_eth_handle_t s_eth_handle = NULL;

static const char *TAG = "ethernet";

/**
 * @note RMII data pins are fixed in esp32:
 * TXD0 <=> GPIO19
 * TXD1 <=> GPIO22
 * TX_EN <=> GPIO21
 * RXD0 <=> GPIO25
 * RXD1 <=> GPIO26
 * CLK <=> GPIO0
 *
 */

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
  case ETHERNET_EVENT_CONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Up");
    break;
  case ETHERNET_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "Ethernet Link Down");
    if (ipnet_lostIpAddr_cb)
      ipnet_lostIpAddr_cb();
    break;
  case ETHERNET_EVENT_START:
    ESP_LOGI(TAG, "Ethernet Started");
    break;
  case ETHERNET_EVENT_STOP:
    ESP_LOGI(TAG, "Ethernet Stopped");
    break;
  default:
    break;
  }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
  const esp_netif_ip_info_t *ip_info = &event->ip_info;

  ESP_LOGI(TAG, "Ethernet Got IP Address");
  ESP_LOGI(TAG, "~~~~~~~~~~~");
  ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
  ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
  ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
  ESP_LOGI(TAG, "~~~~~~~~~~~");

  ip4_address = ip_info->ip;
  ip4_gateway_address = ip_info->gw;
  ip4_netmask = ip_info->netmask;

  if (ipnet_gotIpAddr_cb)
    ipnet_gotIpAddr_cb();

  mainLoop_callFun(ipnet_connected);
}

static void ethernet_configure(enum lanPhy lan_phy, int lan_pwr_gpio) {

  switch (lan_phy) {
  case lanPhyRTL8201:
    ethernet_create_phy = esp_eth_phy_new_rtl8201;
    break;
  case lanPhyIP101:
    ethernet_create_phy = esp_eth_phy_new_ip101;
    break;
  case lanPhyLAN8720:
  default:
    ethernet_create_phy = esp_eth_phy_new_lan87xx;
    break;
  }

  ethernet_phy_power_pin = lan_pwr_gpio;

}

void ethernet_setdown() {
  if (!s_eth_handle)
    return;
  ESP_ERROR_CHECK(esp_eth_stop(s_eth_handle));

  s_eth_handle = 0;
  esp_netif_destroy(eth_netif);
  eth_netif = 0;
}

void ethernet_setup(struct cfg_lan *cfg_lan) {
  if (s_eth_handle)
    ethernet_setdown();

  ethernet_configure(cfg_lan->phy, cfg_lan->pwr_gpio);

  esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
  eth_netif = esp_netif_new(&cfg);

  ESP_ERROR_CHECK(esp_event_handler_instance_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL, NULL));

  // power on phy here
  if (ethernet_phy_power_pin >= 0) {
    gpio_pad_select_gpio(ethernet_phy_power_pin);
    gpio_set_direction(static_cast<gpio_num_t>(ethernet_phy_power_pin), GPIO_MODE_OUTPUT);
    gpio_set_level(static_cast<gpio_num_t>(ethernet_phy_power_pin), 1);
    vTaskDelay(pdMS_TO_TICKS(300));
  }

  // Setup MAC
  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();

  eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
  mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);

  // Setup PHY
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

  phy_config.phy_addr = 0;
  phy_config.reset_gpio_num = -1;
  phy = ethernet_create_phy(&phy_config);

  // Ethernet Driver
  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
  if (esp_eth_driver_install(&config, &s_eth_handle) == ESP_OK) {
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(s_eth_handle)));
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(s_eth_handle));
  } else {
    ESP_LOGI(TAG, "driver install failed");
  }
}

