set(srcs esp32/ota.cc)


idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    PRIV_REQUIRES app_config txtio misc
                  esp_https_ota app_update
)

component_compile_options(-${component_compile_opts})