set(srcs  src/http_server.cc
 esp32/http_server.cc esp32/uri_handlers.cc
)

if(DEFINED CONFIG_APP_USE_WS)
  list(APPEND srcs esp32/websocket.cc)
endif()


include("${CMAKE_CURRENT_LIST_DIR}/../source_filter.cmake" OPTIONAL)

if(COMMAND idf_component_register)
idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "include" 
    PRIV_INCLUDE_DIRS "src" 
    REQUIRES   esp_http_server
    PRIV_REQUIRES  cli utils_debug uout
                    esp_wifi mbedtls #PRIV_ESP_IDF
 )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

else()
add_library(net_http_server STATIC ${srcs})
target_include_directories(net_http_server PUBLIC include PRIVATE src)
target_link_libraries(net_http_server PUBLIC esp_http_server PRIVATE cli utils_debug uout)
endif()