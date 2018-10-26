cmake_minimum_required(VERSION 3.0)

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

add_executable(lua-format ${src_dir})

target_link_libraries(lua-format antlr4-runtime)
