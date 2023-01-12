set(srcs config_kvs.cc config_comps.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/config_kvs" 
    REQUIRES   utils_misc app_config
    PRIV_REQUIRES key_value_store txtio net_mqtt net net_http_server uout
    )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})