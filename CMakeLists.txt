cmake_minimum_required(VERSION 3.0)

project("lua-formatter")

option(BUILD_TESTS "set ON to build tests" ON)

if(CMAKE_VERSION VERSION_LESS "3.1")
  add_definitions(-std=c++11)
else()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include_directories(
  /usr/local/include/antlr4-runtime/
  ${PROJECT_SOURCE_DIR}/generated/
  ${PROJECT_SOURCE_DIR}/
  )

set(src_dir
  ${PROJECT_SOURCE_DIR}/lua-format.cpp
  ${PROJECT_SOURCE_DIR}/FormatVisitor.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaLexer.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaParser.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaVisitor.cpp
  ${PROJECT_SOURCE_DIR}/generated/LuaBaseVisitor.cpp
)

add_library(lua-format-lib ${src_dir})
target_link_libraries(lua-format-lib antlr4-runtime)

add_executable(lua-format main.cpp)
set_target_properties(lua-format PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(lua-format lua-format-lib)

if(BUILD_TESTS)
  enable_testing()

  add_executable(lua-format-test test/test_format.cpp)
  set_target_properties(lua-format-test PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(lua-format-test gtest gtest_main)
  target_link_libraries(lua-format-test lua-format-lib)

  add_test(all_tests lua-format-test)
endif()