# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
