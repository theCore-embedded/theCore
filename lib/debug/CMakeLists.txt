add_library(dbg assert.cpp)
target_include_directories(dbg PUBLIC export types)
target_link_libraries(dbg PRIVATE libcpp ${PLATFORM_NAME})
