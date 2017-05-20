add_library(newlib_stubs INTERFACE)
target_sources(newlib_stubs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stubs.cpp)
target_link_libraries(newlib_stubs INTERFACE platform_common ${CONFIG_PLATFORM_NAME})
