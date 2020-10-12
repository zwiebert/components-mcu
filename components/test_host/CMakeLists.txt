set(srcs test_runner/setup.c test_runner/test_runner.c test_runner/mock.cc
    catch2.cc
)





idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS test_runner
    #INCLUDE_DIRS unity/src
    REQUIRES utils_misc
)

add_subdirectory(external/catch2)
target_link_libraries(test_host PUBLIC Catch2)

add_subdirectory(external/unity)
target_link_libraries(test_host PUBLIC unity)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
