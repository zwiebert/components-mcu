set(srcs esp32/ota.cc)


idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    PRIV_REQUIRES app_config txtio utils_misc
                  esp_https_ota app_update
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})