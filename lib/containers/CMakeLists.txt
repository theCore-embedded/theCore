# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(containers INTERFACE)
target_include_directories(containers INTERFACE export)
target_link_libraries(containers INTERFACE utils)
add_unit_host_test(NAME list
	SOURCES tests/list_unit.cpp
	INC_DIRS export
	DEPENDS utils)
