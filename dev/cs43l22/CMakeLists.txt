# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(cs43l22 INTERFACE)
target_link_libraries(cs43l22 INTERFACE types utils bus)
target_include_directories(cs43l22 INTERFACE export)

theCore_create_cog_runner(
    IN      ${CMAKE_CURRENT_LIST_DIR}/templates/cs43l22_cfg.in.hpp
    OUT     ${CORE_GEN_DIR}/export/dev/cs43l22_cfg.hpp
    DEPENDS ${THECORE_TARGET_CONFIG_FILE}
    ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

add_custom_target(cs43l22_generated DEPENDS ${CORE_GEN_DIR}/export/dev/cs43l22_cfg.hpp)
add_dependencies(cs43l22 cs43l22_generated)
