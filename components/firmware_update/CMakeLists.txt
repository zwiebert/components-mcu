set(srcs esp32/ota.cc)


idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    PRIV_REQUIRES  txtio utils_misc
                  esp_https_ota app_update esp_event #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})