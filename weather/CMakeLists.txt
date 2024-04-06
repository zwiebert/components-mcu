set(srcs weather.cc openweathermap.cc weather_irrigation.cc)

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    REQUIRES net
    PRIV_REQUIRES jsmn kvs utils_misc
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})


else()
add_library(weather STATIC ${srcs})
target_include_directories(weather PUBLIC ./include)
target_link_libraries(weather PUBLIC net PRIVATE jsmn kvs utils_misc)

endif()