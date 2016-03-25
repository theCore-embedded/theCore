add_library(allocators STATIC alloc.cpp)
target_include_directories(allocators PUBLIC export)
# Requires assert
target_link_libraries(allocators utils)
# Static analysis
add_cppcheck(allocators UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
# Unit tests
add_unit_host_test(NAME test_pool_allocator
	SOURCES
	tests/pool_base_unit.cpp
	tests/pool_unit.cpp
	tests/pool_main.cpp
	alloc.cpp
	INC_DIRS export
	DEPENDS utils libcpp)

# Substitude standart headers
target_include_directories(test_pool_allocator BEFORE PRIVATE test)
