# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

file(MAKE_DIRECTORY ${CORE_GEN_DIR}/export/aux)

add_library(ke02 platform.cpp)

include(mcu_cfg.cmake)

target_link_libraries(ke02 PUBLIC arch cmsis kexx_drv platform_common dbg utils containers)

target_include_directories(ke02 PUBLIC export)

# Validate incoming JSON
theCore_create_json_validator_runner(
    NAME ke02_validate
    JSON ${THECORE_TARGET_CONFIG_FILE}
    SCHEMA ${CMAKE_CURRENT_LIST_DIR}/schemas/ke02.schema.json
    SUBOBJECT "platform"
    WORKDIR ${CMAKE_CURRENT_LIST_DIR}/schemas
)

#-------------------------------------------------------------------------------

# Convenient function to generate ke02 platform files.
function(generate_source IN_NAME OUT_NAME)
    # Place header in separate directory
    if(OUT_NAME MATCHES "\.hpp$")
        set(OUT_DIR ${CORE_GEN_DIR}/export/aux)
    else()
        set(OUT_DIR ${CORE_GEN_DIR})
    endif()

    set(OUT_PATH "${OUT_DIR}/${OUT_NAME}")
    set(IN_PATH "${CMAKE_CURRENT_LIST_DIR}/templates/${IN_NAME}")
    list(APPEND GENERATED_FILES ${OUT_PATH})
    set(GENERATED_FILES ${GENERATED_FILES} PARENT_SCOPE)

    theCore_create_cog_runner(
        IN      ${IN_PATH}
        OUT     ${OUT_PATH}
        DEPENDS ${THECORE_TARGET_CONFIG_FILE} ${IN_PATH}
        ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")
endfunction()

#-------------------------------------------------------------------------------

# Generate platform definitions.
generate_source(defines.in.hpp platform_defines.hpp)

# Generate UART definitions.
generate_source(uart_cfg.in.hpp uart_cfg.hpp)

#-------------------------------------------------------------------------------

# Enable console driver.

msg_trace("KE02: Checking console configuration")
if(DEFINED thecore_cfg.platform.console)
    msg_info("Console driver will be used")
    # Bypass console must be enabled automatically.
    set(THECORE_CONFIG_USE_CONSOLE 1 CACHE FORCE "Console enable flag")
    target_sources(ke02 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/console.cpp)
endif()

if(NOT CONFIG_OS)
    target_compile_definitions(ke02 PUBLIC -DTHECORE_NO_OS=1)
endif()

#-------------------------------------------------------------------------------

# Tie all generated code together.
add_custom_target(platform_generated DEPENDS ke02_validate ${GENERATED_FILES})
add_dependencies(ke02 platform_generated)
target_include_directories(ke02 PUBLIC ${CORE_GEN_DIR}/export/)
