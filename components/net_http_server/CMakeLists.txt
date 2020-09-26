set(srcs http_server.cc  esp32/http_server.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES  app_config esp_http_server
    PRIV_REQUIRES  cli dbg uout
                    mbedtls #PRIV_ESP_IDF
 )
 
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})