set(srcs  ipnet.c 
esp32/ethernet.c  esp32/ethernet/esp_eth_phy_lan8720.c
esp32/ipnet.c  esp32/ntp.c  esp32/tcp_server.c  esp32/tcp_cli_server_task.cc esp32/wifi_ap.c  esp32/wifistation.c esp32/http_client.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES ""
    PRIV_REQUIRES app_config misc net_http_server txtio cli debug
                  esp_netif esp_wifi esp_eth esp_http_client
 )
