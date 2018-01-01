# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

find_package(cmake-version4git)

add_library(utils STATIC ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
target_include_directories(utils INTERFACE export)

add_unit_host_test(
    NAME utils
    SOURCES tests/utils_unit.cpp
    INC_DIRS export
)

if(cmake-version4git_FOUND)
	PROJECT_VERSION_FROM_GIT()
	# Better shortcut
	set(THECORE_VER4GIT_FOUND 1)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.in.cpp"
	"${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY)
