add_library(containers INTERFACE)
target_include_directories(containers INTERFACE export)
add_unit_host_test(NAME list
	SOURCES tests/list_unit.cpp
	INC_DIRS export)
