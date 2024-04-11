set(srcs  ipnet.cc
esp32/ipnet.c  esp32/ntp.cc
# esp32/tcp_server.cc
  esp32/tcp_cli_server_task.cc esp32/wifi_ap.cc
  esp32/http_client.cc) 

if(TEST_HOST)
  list(APPEND srcs host/http_client.cc)
endif()

if(DEFINED CONFIG_APP_USE_WLAN)
  list(APPEND srcs esp32/wifistation.cc)
endif()
if(DEFINED CONFIG_APP_USE_LAN)
  list(APPEND srcs esp32/ethernet.cc)
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES uout
    PRIV_REQUIRES main_loop  utils_misc net_http_server txtio cli utils_debug uout
                  esp_netif esp_wifi esp_eth esp_http_client #PRIV_ESP_IDF
 )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})