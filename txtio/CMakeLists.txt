set(srcs inout.cc  txtio_mutex.cc) 
set(platform_srcs esp32/txtio_setup.cc) 

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
idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES utils_misc
    PRIV_REQUIRES uout
                  driver vfs #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

else()
add_library(txtio STATIC ${srcs})
target_include_directories(txtio PUBLIC ./include)
target_link_libraries(txtio PUBLIC utils_misc PRIVATE uout)

endif()