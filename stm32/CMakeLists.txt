set(srcs stm32.cc stm32_flash.cc stm32_bootloader.cc stm32_bl.cc
)
set(platform_srcs
   esp32/mutex.cc esp32/stm32_uart.cc esp32/stm32_impl.cc esp32/stm32_flash.cc
)

if(NOT DEFINED ${TARGET_PLATFORM_FILTER})
  if(COMMAND idf_component_register)
    set(TARGET_PLATFORM_FILTER "esp32/.*")
  elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
    set(TARGET_PLATFORM_FILTER "")
    message("please set TARGET_PLATFORM_FILTER for <${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}>")
    set(TARGET_PLATFORM_FILTER "NONE")
  else()
    set(TARGET_PLATFORM_FILTER "host/.*")
  endif()
endif()
message("TARGET_PLATFORM_FILTER regex is: <${TARGET_PLATFORM_FILTER}>")
list(FILTER platform_srcs INCLUDE REGEX "${TARGET_PLATFORM_FILTER}")
list(APPEND srcs ${platform_srcs})


if(COMMAND idf_component_register)
idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES ""  
    PRIV_REQUIRES net utils_debug utils_misc
                  driver 
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

else()
add_library(stm32 STATIC ${srcs})
target_include_directories(stm32 PUBLIC ./include)
target_link_libraries(stm32 PRIVATE net utils_debug utils_misc)
endif()