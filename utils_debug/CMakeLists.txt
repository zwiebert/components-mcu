set(srcs dbg.cc)

if(COMMAND idf_component_register)
idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include"
   REQUIRES  txtio
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
else()

add_library(utils_debug STATIC ${srcs})
target_include_directories(utils_debug PUBLIC ./include)
target_link_libraries(utils_debug PUBLIC txtio)

endif()