set(srcs  http_server.cc  esp32/http_server.cc esp32/uri_handlers.cc)


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES   esp_http_server
    PRIV_REQUIRES  cli utils_debug uout
                    esp_wifi mbedtls #PRIV_ESP_IDF
 )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
