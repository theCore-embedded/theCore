# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(tm4c platform.cpp exti_manager.cpp)

include(mcu_cfg.cmake)

target_link_libraries(tm4c PUBLIC
    arch cmsis tivaware platform_common dbg utils containers)

target_include_directories(tm4c PUBLIC export)

# Enable console driver.

msg_trace("TM4C: Checking [CONFIG_USE_CONSOLE] and [CONFIG_USE_BYPASS_CONSOLE]...")
if(CONFIG_USE_CONSOLE)
    msg_info("Regular console driver will be used")
    # Bypass console must be enabled automatically.
    set(CONFIG_USE_BYPASS_CONSOLE 1)

    target_compile_definitions(tm4c PUBLIC -DCORE_CONFIG_USE_CONSOLE)
else()
    msg_info("Only bypass console is enabled")
endif()

if(CONFIG_USE_BYPASS_CONSOLE)
    # TODO: #83 - config name must be reviewed
    if(NOT DEFINED CONFIG_CONSOLE_DEVICE_NUM)
        msg_fatal("CONFIG_CONSOLE_DEVICE_NUM must be set in order to use console")
    endif()

    msg_info("Console device used: ${CONFIG_CONSOLE_DEVICE_NUM}")

    # Bypass must be enabled and inited
    target_compile_definitions(tm4c PUBLIC
        -DCORE_CONFIG_USE_BYPASS_CONSOLE
        -DCORE_CONFIG_CONSOLE_DEVICE_NUM=${CONFIG_CONSOLE_DEVICE_NUM})

    # Bypass console implementation
    target_sources(tm4c PRIVATE console.cpp)

    msg_trace("Bypass console is supported and enabled by the platform")
endif()

# Platform is not CMSIS-compliant
set(TARGET_PLATFORM_IS_NOT_CMSIS_COMPLIANT 1 CACHE BOOL "Compliance status" FORCE)

# Represents default value for the THECORE_CONFIG_SYSTMR_FREQ (if SYSTMR is enabled)
# The value of the THECORE_CONFIG_SYSTMR_FREQ must be in the range [20Hz, 1000Hz]
set(THECORE_DEFAULT_SYSTMR_FREQ 50)

# Use systick unless otherwise is explicitly stated
if(NOT CONFIG_OS AND NOT DEFINED CONFIG_USE_SYSTMR_SYSTICK)
    set(CONFIG_USE_SYSTMR_SYSTICK 1)
endif()

if(CONFIG_USE_SYSTMR_SYSTICK)
    if (NOT THECORE_CONFIG_SYSTMR_FREQ)
        set(THECORE_CONFIG_SYSTMR_FREQ ${THECORE_DEFAULT_SYSTMR_FREQ})
    endif()

    target_compile_definitions(tm4c PUBLIC -DUSE_SYSTMR=1 -DTHECORE_CONFIG_SYSTMR_FREQ=${THECORE_CONFIG_SYSTMR_FREQ})
endif()

# Platform provides its own implementation of WFI/WFE routines.
target_compile_definitions(tm4c PUBLIC -DUSE_WFI_WFE=1)
