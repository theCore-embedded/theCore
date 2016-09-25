add_library(bus INTERFACE)
target_include_directories(bus INTERFACE export)
target_link_libraries(bus INTERFACE dbg platform_common thread)

add_unit_host_test(NAME bus
				   SOURCES tests/bus_unit.cpp
				   DEPENDS thread_common platform_common dbg
				   INC_DIRS export tests/mocks)
