cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)

set(TARGET_PLATFORM_FILTER "host/.*")

if (BUILD_HOST_TESTS)
project(tfmcu-test)
set(proj_src_dir "${CMAKE_SOURCE_DIR}/../..")

message("proj_src_dir: ${proj_src_dir}")

add_compile_definitions(USE_JSON)

set(COMPONENT_DIRECTORIES . _test_/components)
list(TRANSFORM COMPONENT_DIRECTORIES PREPEND   "${proj_src_dir}/")

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts -O0 -ggdb  # -fanalyzer
)

if(${GPROF})
  add_link_options(-pg)
  add_compile_options(-pg)
endif()

if(ASAN)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fstack-protector-all)
endif()

include(${proj_src_dir}/test_host/host_test.cmake)

add_subdirectory(${proj_src_dir}/_test_/components/main ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/main)

message("all_inc_dirs: ${COMPONENT_LIBS_INC_DIRS}")
message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")

doxy_create_input()

endif() # BUILD_HOST_TESTS

if (BUILD_DOXY)
 message("config: ${DOXY_CONFIG}")
endif()
