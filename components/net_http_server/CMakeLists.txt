set(srcs http_server.cc  esp32/http_server.c) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES  app_config esp_http_server
    PRIV_REQUIRES  cli debug userio http_server_content
                    mbedtls
 )
 
component_compile_options(${component_compile_opts})