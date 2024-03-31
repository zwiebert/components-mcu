set(srcs stm32.cc esp32/mutex.cc esp32/stm32_uart.cc 
  esp32/stm32_impl.cc stm32_flash.cc esp32/stm32_flash.cc stm32_bootloader.cc
  stm32_bl.cc
)


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES ""  
    PRIV_REQUIRES  net utils_debug utils_misc
                  spiffs driver esp_rom 
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
