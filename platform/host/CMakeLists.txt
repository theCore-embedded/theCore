# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(host console.cpp)
add_library(startup INTERFACE)

# Validate incoming JSON
add_custom_target(host_validate
    COMMAND python ${CMAKE_CURRENT_LIST_DIR}/validate.py ${THECORE_TARGET_CONFIG_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/schemas
    VERBATIM
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/validate.py ${THECORE_TARGET_CONFIG_FILE}
    COMMENT "Validating platform configuration"
)

# Check if console must be enabled.
if(thecore_cfg.platform.console)
    set(THECORE_CONFIG_USE_CONSOLE 1 CACHE STRING "host console" FORCE)
endif()

# Export platform definitions

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/templates/defines.in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/export/aux/platform_defines.hpp
)

target_include_directories(host PUBLIC
    export export/platform ${CMAKE_CURRENT_BINARY_DIR}/export/)
target_link_libraries(host PUBLIC types)
target_link_libraries(host PUBLIC platform_common)

add_dependencies(host host_validate)
