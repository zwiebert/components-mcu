set(srcs test_runner/setup.c test_runner/test_runner.c test_runner/mock.cc
)

if(HOST_TEST_CATCH2)
  list(APPEND srcs  catch2.cc)
endif()


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS test_runner
    #INCLUDE_DIRS unity/src
    REQUIRES utils_misc
)

if(HOST_TEST_CATCH2)
  add_subdirectory(external/catch2)
  target_link_libraries(test_host PUBLIC Catch2)
endif()

if(HOST_TEST_UNITY)
  add_subdirectory(external/unity)
  target_link_libraries(test_host PUBLIC unity)
endif()

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
