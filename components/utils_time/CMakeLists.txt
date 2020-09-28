set(srcs periodic.c)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_REQUIRES app_config utils_misc
)
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})