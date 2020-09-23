set(srcs  ipnet.cc
esp32/ethernet.c  esp32/ethernet/esp_eth_phy_lan8720.c
esp32/ipnet.c  esp32/ntp.c  esp32/tcp_server.c  esp32/tcp_cli_server_task.cc esp32/wifi_ap.c  esp32/wifistation.cc esp32/http_client.c) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES ""
    PRIV_REQUIRES app_config misc net_http_server txtio cli debug
                  esp_netif esp_wifi esp_eth esp_http_client #PRIV_ESP_IDF
 )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})