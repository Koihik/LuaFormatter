cmake_minimum_required(VERSION 3.9)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

project("lua-formatter" VERSION 1.3.6 LANGUAGES CXX)

option(BUILD_TESTS "set ON to build tests" ON)
option(COVERAGE "set ON to enable coverage" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto OUTPUT error)

if(lto)
  message(STATUS "IPO / LTO enabled")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

# The code is modified from
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Write-Platform-Checks
include(CheckIncludeFileCXX)
check_include_file_cxx("filesystem" HAVE_FILESYSTEM_H)
if(NOT HAVE_FILESYSTEM_H)
    message(WARNING "filesystem not found, use experimental/filesystem as fallback."
        " It's recommended to use a stable C++17 compliant compiler/libstdc++.")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigCheck.h)

if(NOT CMAKE_BUILD_TYPE)
  message("set build type to Release.")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
    ((CMAKE_SYSTEM_NAME STREQUAL "Linux") OR (CMAKE_CXX_COMPILER_VERSION LESS 9))))
  set(extra-libs "stdc++fs")
else()
  set(extra-libs "")
endif()

try_run(TEST_RUN_RESULT
  TEST_COMPILE_RESULT
  ${CMAKE_CURRENT_BINARY_DIR}/
  ${PROJECT_SOURCE_DIR}/test/cpp17/fs.cpp
  LINK_LIBRARIES ${extra-libs}
  OUTPUT_VARIABLE var)

message("${var}")

if(NOT "${TEST_COMPILE_RESULT}" OR (NOT "${TEST_RUN_RESULT}" EQUAL 0))
  message(FATAL_ERROR "Your compiler does not fully support the C++17 standard and libraries")
endif()

if( ${CMAKE_BUILD_TYPE} MATCHES "Debug")
  add_definitions(-DDEBUG)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/generated/
  ${PROJECT_SOURCE_DIR}/third_party/
  ${PROJECT_SOURCE_DIR}/third_party/Catch2/single_include
  ${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/include
  ${PROJECT_SOURCE_DIR}/third_party/antlr4/runtime/Cpp/runtime/src
  ${PROJECT_SOURCE_DIR}/src/
  )

file(GLOB_RECURSE src_dir
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/generated/*.cpp
  )

list(REMOVE_ITEM src_dir ${PROJECT_SOURCE_DIR}/src/main.cpp)

file(GLOB_RECURSE antlr4-cpp-src
  ${PROJECT_SOURCE_DIR}/third_party/antlr4/runtime/Cpp/runtime/src/*.cpp
  )

file(GLOB_RECURSE yaml-cpp-src
  ${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/src/*.cpp
  )

add_library (antlr4-cpp-runtime STATIC ${antlr4-cpp-src})
add_library (yaml-cpp STATIC ${yaml-cpp-src})

add_executable(lua-format ${src_dir} src/main.cpp)

target_compile_definitions(lua-format PRIVATE LUA_FORMAT_VERSION="${CMAKE_PROJECT_VERSION}")

if(WIN32)
  target_compile_definitions(antlr4-cpp-runtime PUBLIC ANTLR4CPP_STATIC)
  target_compile_definitions(yaml-cpp PUBLIC YAMLCPP_STATIC)
endif()

set_target_properties(lua-format PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(lua-format yaml-cpp antlr4-cpp-runtime ${extra-libs})

install(TARGETS lua-format
  RUNTIME DESTINATION bin
  )

if(APPLE AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(aarch64|arm64)$")
    set(BUILD_TESTS OFF)
endif()

if(BUILD_TESTS)
  message("BUILD_TESTS enabled.")
  enable_testing()

  add_executable(lua-format-test
    ${src_dir}
    test/test.cpp
    test/test_config.cpp
    test/test_format_file.cpp
    test/test_parser.cpp
    test/test_args.cpp
    test/test_validation.cpp
    test/test_line_separator.cpp
    )
  set_target_properties(lua-format-test PROPERTIES LINKER_LANGUAGE CXX)

  if(COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug")
    message("COVERAGE enabled.")
    set_target_properties(lua-format-test
      PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}"
      LINK_FLAGS "-fprofile-arcs"
      )
  endif()

  target_compile_definitions(lua-format-test PUBLIC PROJECT_PATH="${PROJECT_SOURCE_DIR}")
  target_link_libraries(lua-format-test yaml-cpp antlr4-cpp-runtime ${extra-libs})

  add_test(NAME args COMMAND lua-format-test [args])
  add_test(NAME config COMMAND lua-format-test [config])
  add_test(NAME line_separator COMMAND lua-format-test [line_separator])
  add_test(NAME parser COMMAND lua-format-test [parser])
  add_test(NAME config_validator COMMAND lua-format-test [config_validator])

  add_test(NAME linebreak COMMAND lua-format-test [linebreak])
  add_test(NAME comment COMMAND lua-format-test [comment])
  add_test(NAME statement COMMAND lua-format-test [statement])
  add_test(NAME literals COMMAND lua-format-test [literals])
  add_test(NAME syntax COMMAND lua-format-test [syntax])
  add_test(NAME expression COMMAND lua-format-test [expression])
  add_test(NAME disable_format COMMAND lua-format-test [disable_format])
  add_test(NAME keep_simple_block_one_line COMMAND lua-format-test [keep_simple_block_one_line])
  add_test(NAME issues COMMAND lua-format-test [issues])
endif()
