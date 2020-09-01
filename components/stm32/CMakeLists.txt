set(srcs esp32/stm32_bl.c  esp32/stm32.c  esp32/stm32_ota.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES ""  
    PRIV_REQUIRES app_config net debug
                  spiffs
)