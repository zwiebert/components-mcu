set(srcs esp32/stm32_bl.c  esp32/stm32.c  esp32/stm32_ota.cc
)


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES ""  
    PRIV_REQUIRES app_config net debug
                  spiffs
)

component_compile_options(-${component_compile_opts})