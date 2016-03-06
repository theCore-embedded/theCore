add_library(bus INTERFACE)
target_include_directories(bus INTERFACE export)
target_link_libraries(bus INTERFACE utils)

add_unit_host_test(NAME bus_test
				   SOURCES tests/bus_unit.cpp
				   DEPENDS thread_common
				   INC_DIRS export tests/mocks)
