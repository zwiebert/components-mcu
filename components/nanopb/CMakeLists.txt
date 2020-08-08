set(srcs 
  nanopb/pb_common.h
  nanopb/pb_encode.c
  nanopb/pb_decode.c
)

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "." #"./include" 
)
