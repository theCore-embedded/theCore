add_library(bus INTERFACE)
target_include_directories(bus INTERFACE export)
target_link_libraries(bus INTERFACE dbg)
target_link_libraries(bus INTERFACE common_bus)

add_unit_host_test(NAME bus
				   SOURCES tests/bus_unit.cpp
				   DEPENDS thread_common common_bus dbg
				   INC_DIRS export tests/mocks)
