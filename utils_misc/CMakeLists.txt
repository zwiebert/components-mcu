set(srcs bcd.c  stof.c  sun.c base64.c esp32/itoa.c ftoa.c cstring_utils.cc esp32/mutex.cc)

if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs}
    REQUIRES 
    PRIV_REQUIRES utils_debug
    INCLUDE_DIRS "./include"
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

else()
list(FILTER srcs EXCLUDE REGEX "(esp32|esp8266|atmega*)/.*")
add_library(utils_misc STATIC ${srcs})
target_include_directories(utils_misc PUBLIC ./include)

endif()