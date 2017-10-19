# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(sys INTERFACE)
target_sources(sys INTERFACE ${CMAKE_CURRENT_LIST_DIR}/sys.cpp)

# Call to the platform_init() from platform module is required.
target_link_libraries(sys INTERFACE ${PLATFORM_NAME})
# IRQ manager is required.
target_link_libraries(sys INTERFACE platform_common)

# A special initialization is required if kernel is present
if(DEFINED CONFIG_OS)
	target_link_libraries(sys INTERFACE ${CONFIG_OS}_main)
else()
	target_sources(sys INTERFACE ${CMAKE_CURRENT_LIST_DIR}/kernel_stubs.c)
endif()
