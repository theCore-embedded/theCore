add_library(hm10 INTERFACE)
target_include_directories(hm10 INTERFACE export)
target_link_libraries(hm10 INTERFACE platform_common types dbg)

add_unit_host_test(NAME hm10
    SOURCES tests/hm10_unit.cpp
	INC_DIRS export
	DEPENDS types platform_common dbg)
