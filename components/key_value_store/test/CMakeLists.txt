idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "." ../include 
 #   PRIV_INCLUDE_DIRS ../include/key_value_store
    REQUIRES unity key_value_store
)
                  
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})