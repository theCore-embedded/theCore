cmake_minimum_required(VERSION 3.4)

# 'Do nothing' test case.
project(thecore_empty_main_case)

include(${TARGET_DIR}/target_defs.cmake)

add_executable(empty_main main.cpp)
target_include_directories(empty_main PUBLIC ${TARGET_DIR})
target_link_libraries(empty_main the_core)

add_subdirectory(${CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/core)
