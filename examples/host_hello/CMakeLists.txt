cmake_minimum_required(VERSION 3.2)

# theCore framework directory.
set(CORE_DIR ../../)

# Required.
include(${CORE_DIR}/build_api.cmake)

# Obvious.
project(host_hello)

# Our demo sources.
add_executable(hello main.cpp)

# Obvious
set(CONFIG_RUN_CPPCHECK_DURING_BUILD true)

# Some dependencies
target_link_libraries(hello the_core)
target_link_libraries(hello core_cpp)
target_link_libraries(hello host)

# Host configuration parameters
theCore_set_target_config(${CMAKE_CURRENT_LIST_DIR}/host.json)

# Include theCore into the build process
add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
