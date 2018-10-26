cmake_minimum_required(VERSION 3.0)

include(CTest)
enable_testing()

add_executable(lua-format-test test.cpp)

set_target_properties(lua-format-test PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(lua-format-test
    ${project_library_target_name} ${REQUIRED_LIBRARIES})

add_test(all_tests lua-format-test)
add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS lua-format-test)
