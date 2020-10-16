idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "." ../include 
    REQUIRES unity uout
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})