
idf_component_register(
    SRCS ""
    REQUIRES  cli config_kvs firmware_update jsmn kvs main_loop nanopb net net_http_server net_mqtt_client stm32 storage test_host txtio uout utils_debug utils_misc utils_time weather
    )


component_compile_options(${comp_compile_opts} -lfreertos)
component_compile_features(${comp_compile_feats})
