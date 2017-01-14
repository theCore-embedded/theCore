add_library(newlib_stubs INTERFACE)
target_sources(newlib_stubs INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stubs.c)
target_link_libraries(newlib_stubs INTERFACE ${CONFIG_PLATFORM_NAME})
