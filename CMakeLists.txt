cmake_minimum_required(VERSION 3.9)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

project("lua-formatter" VERSION 1.3.0 LANGUAGES CXX)

option(BUILD_TESTS "set ON to build tests" ON)
option(COVERAGE "set ON to enable coverage" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto OUTPUT error)

if(lto)
  message(STATUS "IPO / LTO enabled")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

if(NOT CMAKE_BUILD_TYPE)
  message("set build type to Release.")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
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


include_directories(
  ${PROJECT_SOURCE_DIR}/generated/
  ${PROJECT_SOURCE_DIR}/third_party/
  ${PROJECT_SOURCE_DIR}/third_party/Catch2/single_include
  ${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/include
  ${PROJECT_SOURCE_DIR}/third_party/antlr4/runtime/Cpp/runtime/src
  ${PROJECT_SOURCE_DIR}/src/
  )

set(src_dir
  ${PROJECT_SOURCE_DIR}/src/lua-format.cpp
  ${PROJECT_SOURCE_DIR}/src/Config.cpp
  ${PROJECT_SOURCE_DIR}/src/FormatVisitor.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaLexer.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaParser.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaVisitor.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaBaseVisitor.cpp
  )

file(GLOB_RECURSE antlr4-cpp-src
  ${PROJECT_SOURCE_DIR}/third_party/antlr4/runtime/Cpp/runtime/src/*.cpp
  )

file(GLOB_RECURSE yaml-cpp-src
  ${PROJECT_SOURCE_DIR}/third_party/yaml-cpp/src/*.cpp
  )

add_library (antlr4-cpp-runtime ${antlr4-cpp-src})
add_library (yaml-cpp ${yaml-cpp-src})

add_executable(lua-format ${src_dir} src/main.cpp)

if(WIN32)
  target_compile_definitions(antlr4-cpp-runtime PRIVATE ANTLR4CPP_EXPORTS)
  target_compile_definitions(yaml-cpp PRIVATE YAMLCPP_EXPORTS)
endif()

set_target_properties(lua-format PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(lua-format yaml-cpp antlr4-cpp-runtime ${extra-libs})

install(TARGETS lua-format
  RUNTIME DESTINATION bin
  )

if(BUILD_TESTS)
  message("BUILD_TESTS enabled.")
  enable_testing()

  add_executable(lua-format-test
    ${src_dir}
    test/test.cpp
    test/test_config.cpp
    test/test_format_file.cpp
    test/test_parser.cpp
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

  add_test(all_tests lua-format-test)
endif()
