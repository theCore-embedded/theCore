cmake_minimum_required(VERSION 3.4)

project(unity_demo)

include(${CMAKE_CURRENT_LIST_DIR}/../../target_defs.cmake)

add_executable(unity_demo ${AUTOGEN_DIR}/main.cpp suite_init.cpp)

target_link_libraries(unity_demo the_core unity)

# Suite setup

# Enable console.
set(CONFIG_USE_CONSOLE 1)

# Enable OS support, required to enable sleep_for() call
set(CONFIG_OS host)

# Suite tests
include(${AUTOGEN_DIR}/suite_tests.cmake)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
