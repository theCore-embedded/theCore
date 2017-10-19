# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# New-to-old API bridges

set(CONFIG_PLATFORM_DEVICE  "${thecore_cfg.platform.device}"
    CACHE STRING "stm32 device id" FORCE
)

# TODO: what if clock is not HSE?
set(CONFIG_PLATFORM_HSE_VAL "${thecore_cfg.platform.clock.speed}"
    CACHE STRING "HSE speed" FORCE
)

# Check if console must be enabled.
if(thecore_cfg.platform.console)
    set(THECORE_CONFIG_USE_CONSOLE 1 CACHE STRING "stm32 console" FORCE)
endif()

# Create directories required for auto-generated files
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)
file(MAKE_DIRECTORY ${GEN_DIR}/export/aux)

add_library(stm32 platform.cpp exti_manager.cpp console.cpp ${GEN_DIR}/pin_mux.cpp)

# Validate incoming JSON
theCore_create_json_validator_runner(
    NAME stm32_validate
    JSON ${THECORE_TARGET_CONFIG_FILE}
    SCHEMA ${CMAKE_CURRENT_LIST_DIR}/schemas/stm32.schema.json
    WORKDIR ${CMAKE_CURRENT_LIST_DIR}/schemas
)

target_include_directories(stm32 PUBLIC export)

target_link_libraries(stm32 PUBLIC
        stm32_family types platform_common arch containers dbg)

# Get proper MCU definitions
add_subdirectory(family)

msg_trace("Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
    set(CONFIG_MAX_ISR_PRIORITY 0xff)
    msg_trace("CONFIG_MAX_ISR_PRIORITY not set,"
            " using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_compile_definitions(stm32
        PRIVATE
        -DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY}
        -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})

#-------------------------------------------------------------------------------

# Convenient function to generate stm32 platform files.
function(generate_source IN_NAME OUT_NAME)
    # Place header in separate directory
    if(OUT_NAME MATCHES "\.hpp$")
        set(OUT_DIR ${GEN_DIR}/export/aux)
    else()
        set(OUT_DIR ${GEN_DIR})
    endif()

    set(OUT_PATH "${OUT_DIR}/${OUT_NAME}")
    list(APPEND GENERATED_FILES ${OUT_PATH})
    set(GENERATED_FILES ${GENERATED_FILES} PARENT_SCOPE)

    theCore_create_cog_runner(
        IN      ${CMAKE_CURRENT_LIST_DIR}/templates/${IN_NAME}
        OUT     ${OUT_PATH}
        DEPENDS ${THECORE_TARGET_CONFIG_FILE}
        ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")
endfunction()

#-------------------------------------------------------------------------------

# Generate platform definitions.
generate_source(defines.in.hpp platform_defines.hpp)

# Generate UART definitions.
generate_source(uart_cfg.in.hpp uart_cfg.hpp)

# Generate ADC definitions
generate_source(adc_cfg.in.hpp adc_cfg.hpp)

# Generate GPIO definitions
generate_source(gpio_cfg.in.hpp gpio_cfg.hpp)

# Generate I2C definitions
generate_source(i2c_cfg.in.hpp i2c_cfg.hpp)

# Generate SPI and I2S definitions
generate_source(spi_i2s_cfg.in.hpp spi_i2s_cfg.hpp)

# Generate pins definitions
generate_source(pin_mux.in.cpp pin_mux.cpp)

#-------------------------------------------------------------------------------

# Tie all generated code together.

add_custom_target(platform_generated DEPENDS stm32_validate ${GENERATED_FILES})
add_dependencies(stm32 platform_generated)
target_include_directories(stm32 PUBLIC ${GEN_DIR}/export/)
