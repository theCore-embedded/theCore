add_library(dbg INTERFACE)
target_sources(dbg INTERFACE ${CMAKE_CURRENT_LIST_DIR}/assert.cpp)
target_include_directories(dbg INTERFACE export types)
target_link_libraries(dbg INTERFACE ${PLATFORM_NAME})
