add_unit_host_test(
	NAME aux_internal
	SOURCES aux_unit.cpp
	DEPENDS dbg
	INC_DIRS ../export/ecl)

add_unit_host_test(
	NAME shared
	SOURCES shared_unit.cpp
	DEPENDS dbg
	INC_DIRS ../export/ecl)
