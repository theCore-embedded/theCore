# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(htu21d INTERFACE)
target_link_libraries(htu21d INTERFACE types)
target_include_directories(htu21d INTERFACE export)

theCore_create_cog_runner(
    IN      ${CMAKE_CURRENT_LIST_DIR}/templates/htu21d_cfg.in.hpp
    OUT     ${CORE_GEN_DIR}/export/dev/sensor/htu21d_cfg.hpp
    DEPENDS ${THECORE_TARGET_CONFIG_FILE}
    ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

add_custom_target(htu21d_generated DEPENDS ${CORE_GEN_DIR}/export/dev/sensor/htu21d_cfg.hpp)
add_dependencies(htu21d htu21d_generated)
