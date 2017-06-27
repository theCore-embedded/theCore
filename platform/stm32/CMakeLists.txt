add_library(stm32 platform.cpp exti_manager.cpp)

# Get proper MCU definitions
add_subdirectory(family)

msg_trace("Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
    set(CONFIG_MAX_ISR_PRIORITY 0xff)
    msg_trace("CONFIG_MAX_ISR_PRIORITY not set,"
            " using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_include_directories(stm32 PUBLIC export)

target_link_libraries(stm32 PUBLIC stm32_family types platform_common arch containers dbg)

target_compile_definitions(stm32
        PRIVATE
        -DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY}
        -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})

# Pick console driver
msg_trace("STM32: Checking [CONFIG_USE_CONSOLE] and [CONFIG_USE_BYPASS_CONSOLE]...")
if(CONFIG_USE_CONSOLE)
    msg_trace("Regular console driver will be used")
    # Bypass console must be enabled automatically.
    set(CONFIG_USE_BYPASS_CONSOLE 1)

    target_compile_definitions(stm32 PUBLIC -DCONFIG_USE_CONSOLE)
endif()

if(CONFIG_USE_BYPASS_CONSOLE)
    # TODO: #83 - config name must be reviewed
    if(NOT DEFINED CONFIG_CONSOLE_DEVICE_NUM)
        msg_fatal("CONFIG_CONSOLE_DEVICE_NUM must be set in order to use console")
    endif()

    if(NOT DEFINED CONFIG_CONSOLE_BAUD)
        msg_fatal("CONFIG_CONSOLE_BAUD must be set in order to use console")
    endif()

    msg_trace("Console device:  ${CONFIG_CONSOLE_DEVICE_NUM}")
    msg_trace("Console baud rate: ${CONFIG_CONSOLE_BAUD}")

    configure_file(templates/console.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/generated/platform/console.hpp)
    target_include_directories(stm32 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/)

    # Helps to determine if bypass driver must be enabled and inited
    target_compile_definitions(stm32 PUBLIC -DCONFIG_USE_BYPASS_CONSOLE)
    # Bypass console implementation
    target_sources(stm32 PRIVATE console.cpp)
    msg_info("Bypass console is supported and enabled by the platform")
else()
    configure_file(templates/console_stub.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/generated/platform/console.hpp)
    target_include_directories(stm32 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/)
endif()
