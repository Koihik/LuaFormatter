cmake_minimum_required(VERSION 3.0)

project("lua-formatter")

option(BUILD_TESTS "set ON to build tests" ON)
option(COVERAGE "set ON to enable coverage" ON)

if(CMAKE_VERSION VERSION_LESS "3.1")
  add_definitions(-std=c++11)
else()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/generated/
  ${PROJECT_SOURCE_DIR}/third_party/
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/
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

file(GLOB antlr4-cpp-src 
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/atn/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/dfa/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/misc/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/support/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/tree/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/tree/pattern/*.cpp
  ${PROJECT_SOURCE_DIR}/third_party/antlr4-cpp-runtime/tree/xpath/*.cpp
)

add_library (antlr4-cpp-runtime ${antlr4-cpp-src})

add_executable(lua-format ${src_dir} src/main.cpp)
set_target_properties(lua-format PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(lua-format antlr4-cpp-runtime)

if(BUILD_TESTS)
  message("BUILD_TESTS enabled.")
  enable_testing()

  add_executable(lua-format-test ${src_dir} test/test.cpp test/test_config.cpp test/test_format_file.cpp)
  set_target_properties(lua-format-test PROPERTIES LINKER_LANGUAGE CXX)

  if(COVERAGE)
    message("COVERAGE enabled.")
    set_target_properties(lua-format-test
      PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}"
      LINK_FLAGS "-fprofile-arcs"
      )
  endif()

  target_link_libraries(lua-format-test antlr4-cpp-runtime)

  add_test(all_tests lua-format-test)
endif()