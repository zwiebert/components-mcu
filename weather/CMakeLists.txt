set(srcs esp32/weather.cc weather.cc parse_json.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"
    PRIV_REQUIRES jsmn esp_http_client net
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})