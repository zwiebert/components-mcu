set(srcs bcd.c  stof.c  sun.c esp32/itoa.c ftoa.c cstring_utils.cc esp32/mutex.cc)


idf_component_register(SRCS ${srcs}
    REQUIRES app_config
    PRIV_REQUIRES dbg
    INCLUDE_DIRS "./include" 
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})