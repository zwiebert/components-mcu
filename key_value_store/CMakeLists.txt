set(srcs esp32/kvs_implementation.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    REQUIRES nvs_flash
    PRIV_REQUIRES  utils_debug 
                 # nvs_flash #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})