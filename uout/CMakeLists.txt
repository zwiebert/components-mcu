set(srcs status_json.cc callbacks.cc cli_out.cc  so_out.cc so_target_desc.cc)  

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/uout" 
    REQUIRES  utils_debug
    PRIV_REQUIRES  utils_misc
)
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
else()

add_library(uout STATIC ${srcs})
target_include_directories(uout PUBLIC ./include)
target_link_libraries(uout PUBLIC utils_debug PRIVATE utils_misc)

endif()