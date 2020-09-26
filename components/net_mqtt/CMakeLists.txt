set(srcs  mqtt.cc esp32/mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES app_config
    PRIV_REQUIRES cli uout txtio
                  mqtt lwip #PRIV_ESP_IDF
 )
 
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})