cmake_minimum_required(VERSION 3.4)

project(unity_demo)

include(${CMAKE_CURRENT_LIST_DIR}/../../target_defs.cmake)

add_executable(unity_demo ${AUTOGEN_DIR}/main.cpp suite_init.cpp)

target_link_libraries(unity_demo the_core unity)

# Suite setup not needed - all done by target

# Suite tests
include(${AUTOGEN_DIR}/suite_tests.cmake)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
