set(srcs esp32/weather.cc weather.cc parse_json.cc)

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    PRIV_REQUIRES jsmn esp_http_client net
)