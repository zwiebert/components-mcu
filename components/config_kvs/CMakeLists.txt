set(srcs config_kvs.cc config_comps.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/config_kvs" 
    REQUIRES  app_config utils_misc 
    PRIV_REQUIRES key_value_store app_settings txtio net_mqtt net net_http_server app_uout
    )
    
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})