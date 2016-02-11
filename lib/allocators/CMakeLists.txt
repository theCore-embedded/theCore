add_library(allocators STATIC alloc.cpp)
target_include_directories(allocators PUBLIC export)
# Requires assert
target_link_libraries(allocators utils)
# Static analysis
add_cppcheck(allocators UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
