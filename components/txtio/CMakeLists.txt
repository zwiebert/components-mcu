set(srcs inout.cc esp32/txtio_setup.cc txtio_mutex.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/txtio" 
    PRIV_REQUIRES misc app_config uout
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})