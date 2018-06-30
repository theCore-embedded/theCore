# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(hm10 INTERFACE)
target_include_directories(hm10 INTERFACE export)
target_link_libraries(hm10 INTERFACE platform_common bus types dbg)

theCore_create_cog_runner(
    IN      ${CMAKE_CURRENT_LIST_DIR}/templates/hm10_cfg.in.hpp
    OUT     ${CORE_GEN_DIR}/export/dev/hm10_cfg.hpp
    DEPENDS ${THECORE_TARGET_CONFIG_FILE}
    ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

add_custom_target(hm10_generated DEPENDS ${CORE_GEN_DIR}/export/dev/hm10_cfg.hpp)
add_dependencies(hm10 hm10_generated)

add_unit_host_test(NAME hm10
    SOURCES tests/hm10_unit.cpp
    INC_DIRS export tests/mocks
    DEPENDS types platform_common dbg)
