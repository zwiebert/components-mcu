set(srcs cli.cc
 cmdline_get.cc  cmdline_parse.cc  cmdline_set.cc cmdline_process.cc
 mutex.cc esp32/cli_task.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"  
    REQUIRES uout jsmn 
    PRIV_REQUIRES   utils_debug utils_misc txtio uout net_http_server
                  esp_event  #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})