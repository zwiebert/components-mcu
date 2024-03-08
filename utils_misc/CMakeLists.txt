set(srcs bcd.c  stof.c  sun.c esp32/itoa.c ftoa.c cstring_utils.cc esp32/mutex.cc)


idf_component_register(SRCS ${srcs}
    REQUIRES 
    PRIV_REQUIRES utils_debug
    INCLUDE_DIRS "./include"
    PRIV_INCLUDE_DIRS "./include/utils_misc"
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})