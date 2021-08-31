set(srcs esp32/main_queue.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_REQUIRES  utils_debug utils_misc
                  nvs_flash #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})