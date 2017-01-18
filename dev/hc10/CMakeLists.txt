add_library(hc10 INTERFACE)
target_include_directories(hc10 INTERFACE export)
target_link_libraries(hc10 INTERFACE platform_common types dbg)

add_unit_host_test(NAME hc10
	SOURCES tests/hc10_unit.cpp
	INC_DIRS export
	DEPENDS types platform_common dbg)
