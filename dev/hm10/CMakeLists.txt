# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(hm10 INTERFACE)
target_include_directories(hm10 INTERFACE export)
target_link_libraries(hm10 INTERFACE platform_common bus types dbg)

add_unit_host_test(NAME hm10
    SOURCES tests/hm10_unit.cpp
	INC_DIRS export tests/mocks
	DEPENDS types platform_common dbg)
