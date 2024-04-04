set(srcs status_json.cc callbacks.cc cli_out.cc  so_out.cc so_target_desc.cc uo_write_config.cc
#so_mutex.cc
)  

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/uout" 
    REQUIRES  utils_debug
    PRIV_REQUIRES  utils_misc txtio cli net net_mqtt_client config_kvs net_http_server
)
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})