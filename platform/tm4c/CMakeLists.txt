# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

file(MAKE_DIRECTORY ${CORE_GEN_DIR}/export/aux)

add_library(tm4c platform.cpp exti_manager.cpp
    ${CORE_GEN_DIR}/pin_mux.cpp
    ${CORE_GEN_DIR}/irq.cpp)

include(mcu_cfg.cmake)

target_link_libraries(tm4c PUBLIC
    arch cmsis tivaware platform_common dbg utils containers)

target_include_directories(tm4c PUBLIC export)

#-------------------------------------------------------------------------------

# Convenient function to generate tm4c platform files.
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

# Generate GPIO definitions
generate_source(gpio_cfg.in.hpp gpio_cfg.hpp)

# Generate SPI definitions
generate_source(spi_cfg.in.hpp spi_cfg.hpp)

# Generate pins definitions
generate_source(pin_mux.in.cpp pin_mux.cpp)

# Generate IRQ handlers
generate_source(irq.in.cpp irq.cpp)

#-------------------------------------------------------------------------------

# Enable console driver.

msg_trace("TM4C: Checking console configuration")
if(DEFINED thecore_cfg.menu-platform.menu-tm4c.config-console)
    msg_info("Console driver will be used")
    # Bypass console must be enabled automatically.
    set(THECORE_CONFIG_USE_CONSOLE 1 CACHE FORCE "Console enable flag")
    target_sources(tm4c PUBLIC ${CMAKE_CURRENT_LIST_DIR}/console.cpp)
endif()

# Platform is not CMSIS-compliant
set(TARGET_PLATFORM_IS_NOT_CMSIS_COMPLIANT 1 CACHE BOOL "Compliance status" FORCE)

if(NOT CONFIG_OS)
    target_compile_definitions(tm4c PUBLIC -DTHECORE_NO_OS=1)
endif()

#-------------------------------------------------------------------------------

# Tie all generated code together.

add_custom_target(platform_generated DEPENDS ${GENERATED_FILES})
add_dependencies(tm4c platform_generated)
target_include_directories(tm4c PUBLIC ${CORE_GEN_DIR}/export/)
