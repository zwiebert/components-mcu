set(srcs config_kvs.cc comp_settings.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/config_kvs" 
    REQUIRES   utils_misc app_config kvs
    PRIV_REQUIRES uout 
    )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})