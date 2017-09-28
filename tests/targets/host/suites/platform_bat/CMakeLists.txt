# Platform BAT tests for host target.

cmake_minimum_required(VERSION 3.4)

project(platform_bat)

include(${CORE_DIR}/build_api.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../target_defs.cmake)

add_executable(platform_bat ${AUTOGEN_DIR}/main.cpp suite_init.cpp)

target_link_libraries(platform_bat the_core unity utils)

# Suite setup not needed - all done by target

# Suite tests
include(${AUTOGEN_DIR}/suite_tests.cmake)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
