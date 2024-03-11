set(srcs esp32/weather.cc weather.cc openweathermap.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    REQUIRES net
    PRIV_REQUIRES jsmn esp_http_client key_value_store
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})