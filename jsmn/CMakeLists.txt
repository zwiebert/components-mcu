cmake_minimum_required(VERSION 3.16)

set(srcs jsmn.c)
set(incd ./include)

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()


if(COMMAND idf_component_register)
idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS ${incd}
)

else()
add_library(jsmn STATIC ${srcs})
target_include_directories(jsmn PUBLIC ${incd})

endif()
