set(srcs test_runner/setup.c test_runner/test_runner.c test_runner/mock.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS test_runner
    #INCLUDE_DIRS unity/src
    REQUIRES utils_misc
)

add_subdirectory(unity)
target_link_libraries(test_host PUBLIC unity)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
