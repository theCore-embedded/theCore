add_library(tm4c platform.cpp exti_manager.cpp)

include(mcu_cfg.cmake)

target_link_libraries(tm4c PUBLIC
    arch cmsis tivaware platform_common dbg utils containers)

target_include_directories(tm4c PUBLIC export)

# Enable console driver.

message(STATUS "TM4C: Checking [CONFIG_USE_CONSOLE] and [CONFIG_USE_BYPASS_CONSOLE]...")
if(CONFIG_USE_CONSOLE)
    message(STATUS "Regular console driver will be used")
    # Bypass console must be enabled automatically.
    set(CONFIG_USE_BYPASS_CONSOLE 1)

    target_compile_definitions(tm4c PUBLIC -DCORE_CONFIG_USE_CONSOLE)
else()
    message(STATUS "Only bypass console is enabled")
endif()

if(CONFIG_USE_BYPASS_CONSOLE)
    # TODO: #83 - config name must be reviewed
    if(NOT DEFINED CONFIG_CONSOLE_DEVICE_NUM)
        message(FATAL_ERROR "CONFIG_CONSOLE_DEVICE_NUM must be set in order to use console")
    endif()

    message(STATUS "Console device used: ${CONFIG_CONSOLE_DEVICE_NUM}")

    # Bypass must be enabled and inited
    target_compile_definitions(tm4c PUBLIC
        -DCORE_CONFIG_USE_BYPASS_CONSOLE
        -DCORE_CONFIG_CONSOLE_DEVICE_NUM=${CONFIG_CONSOLE_DEVICE_NUM})

    # Bypass console implementation
    target_sources(tm4c PRIVATE console.cpp)

    message(STATUS "Bypass console is supported and enabled by the platform")
endif()

# Platform is not CMSIS-compliant
set(TARGET_PLATFORM_IS_NOT_CMSIS_COMPLIANT 1 CACHE BOOL "Compiancy status" FORCE)

# Platform provides its own implementation of WFI/WFE routines.
target_compile_definitions(tm4c PUBLIC -DUSE_WFI_WFE=1)
