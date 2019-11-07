cmake_minimum_required(VERSION 3.0)

project("lua-formatter" VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_TESTS "set ON to build tests" ON)
option(COVERAGE "set ON to enable coverage" ON)

if(CMAKE_VERSION VERSION_LESS "3.1")
  add_definitions(-std=c++11)
else()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(NOT CMAKE_BUILD_TYPE)
  message("set build type to Release.")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(
  ${PROJECT_SOURCE_DIR}/generated/
  ${PROJECT_SOURCE_DIR}/third_party/
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

add_library (antlr4-cpp-runtime ${antlr4-cpp-src})

add_executable(lua-format ${src_dir} src/main.cpp)

if(WIN32)
  target_compile_definitions(antlr4-cpp-runtime PRIVATE ANTLR4CPP_EXPORTS)
endif()

set_target_properties(lua-format PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(lua-format antlr4-cpp-runtime)

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
  )
  set_target_properties(lua-format-test PROPERTIES LINKER_LANGUAGE CXX)

  if(COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug")
    message("COVERAGE enabled.")
    set_target_properties(lua-format-test
      PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS}"
      LINK_FLAGS "-fprofile-arcs"
      )
  endif()

  target_link_libraries(lua-format-test antlr4-cpp-runtime)

  add_test(all_tests lua-format-test)
endif()
