set(srcs esp32/stm32_bl.cc  esp32/stm32.cc  esp32/stm32_ota.cc  esp32/mutex.cc
)


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES ""  
    PRIV_REQUIRES  net utils_debug utils_misc
                  spiffs driver esp_rom #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
