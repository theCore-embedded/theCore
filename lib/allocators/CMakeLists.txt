add_library(allocators INTERFACE)
target_sources(allocators INTERFACE ${CMAKE_CURRENT_LIST_DIR}/alloc.cpp)
target_include_directories(allocators INTERFACE export)

# Requires assert
target_link_libraries(allocators INTERFACE dbg types)
# Static analysis
add_cppcheck(allocators UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)

# Unit tests
add_unit_host_test(NAME pool_allocator
        SOURCES
        tests/pool_base_unit.cpp
        tests/pool_unit.cpp
        tests/pool_main.cpp
        alloc.cpp
        INC_DIRS export
        DEPENDS libcpp dbg)
