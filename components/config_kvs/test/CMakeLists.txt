idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       REQUIRES app_misc txtio unity app_config
                       PRIV_REQUIRES key_value_store net
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})