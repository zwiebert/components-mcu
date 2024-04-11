set(srcs  esp32/network_connection.cc)

include("${CMAKE_CURRENT_LIST_DIR}/../source_filter.cmake" OPTIONAL)

if(COMMAND idf_component_register)
idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "include" 
    PRIV_REQUIRES  net utils_debug uout config_kvs
                   esp_netif
 )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

else()
add_library(net_connect STATIC ${srcs})
target_include_directories(net_connect PUBLIC include)
target_link_libraries(net_connect PRIVATE net utils_debug uout config_kvs)
endif()