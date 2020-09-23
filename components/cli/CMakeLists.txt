set(srcs cli.cc
 cmdline_get.cc  cmdline_parse.cc  cmdline_set.cc cmdline_process.cc
 mutex.cc esp32/cli_task.cc) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"  
    REQUIRES uout 
    PRIV_REQUIRES app_config  debug misc txtio uout jsmn net_http_server
)