cmake_minimum_required(VERSION 2.8)

project("lua-formatter")

if (NOT WIN32)
  set(CMAKE_CXX_FLAGS "-Wdeprecated -Wno-attributes" )
endif()

if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wdeprecated -std=c++11" )
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "/EHsc /wd4127 /wd4311 /wd4312 /wd4512 /wd4571 /wd4640 /wd4706 /wd4710 /wd4800 /wd4804 /wd4820 /wd4996")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/misc
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/atn
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/dfa
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/tree
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/support
  ${PROJECT_SOURCE_DIR}/
  )

set(src_dir
  ${PROJECT_SOURCE_DIR}/lua-format.cpp
  ${PROJECT_SOURCE_DIR}/FormatVisitor.cpp
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/LuaLexer.cpp
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/LuaParser.cpp
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/LuaVisitor.cpp
  ${PROJECT_SOURCE_DIR}/antlr4-runtime/LuaBaseVisitor.cpp
)

add_executable(lua-format ${src_dir})

if(WIN32)
  target_link_libraries(lua-format "${PROJECT_SOURCE_DIR}/libs/antlr4-runtime.lib")
endif()

if(APPLE)
  target_link_libraries(lua-format "${PROJECT_SOURCE_DIR}/libs/libantlr4-runtime.dylib")
elseif(UNIX)
  target_link_libraries(lua-format "${PROJECT_SOURCE_DIR}/libs/libantlr4-runtime.so")
endif()  
