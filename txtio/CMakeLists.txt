set(srcs inout.cc esp32/txtio_setup.cc txtio_mutex.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/txtio" 
    REQUIRES utils_misc
    PRIV_REQUIRES uout
                  driver vfs #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})