set(srcs esp32/stm32_bl.c  esp32/stm32.c  esp32/stm32_ota.cc
)


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES ""  
    PRIV_REQUIRES app_config net dbg
                  spiffs #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})