# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(sdspi INTERFACE)
target_include_directories(sdspi INTERFACE export)
target_link_libraries(sdspi INTERFACE utils)

theCore_create_cog_runner(
    IN      ${CMAKE_CURRENT_LIST_DIR}/templates/sdspi_cfg.in.hpp
    OUT     ${CORE_GEN_DIR}/export/dev/sdspi_cfg.hpp
    DEPENDS ${THECORE_TARGET_CONFIG_FILE}
    ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

add_custom_target(sdspi_generated DEPENDS ${CORE_GEN_DIR}/export/dev/sdspi_cfg.hpp)
add_dependencies(sdspi sdspi_generated)
