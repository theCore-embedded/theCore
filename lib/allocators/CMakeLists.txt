## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(alloc_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(allocators STATIC alloc.cpp)
target_include_directories(allocators PUBLIC export)
# Requires assert
target_link_libraries(allocators libcpp)
