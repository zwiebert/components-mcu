set(srcs dbg.cc)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include"
   REQUIRES  txtio
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
