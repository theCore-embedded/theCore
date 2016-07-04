add_library(stm32l1xx
        platform.cpp
        irq_manager.cpp
        system_stm32l1xx.c)

# Get proper MCU definitions
include(mcu_cfg.cmake)

message(STATUS "Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
    set(CONFIG_MAX_ISR_PRIORITY 0xff)
    message(STATUS "CONFIG_MAX_ISR_PRIORITY not set,"
            " using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_include_directories(stm32l1xx PUBLIC export)
target_link_libraries(stm32l1xx PUBLIC cmsis)
target_link_libraries(stm32l1xx PUBLIC types)
target_link_libraries(stm32l1xx PUBLIC stm32l1xx_spl)
target_link_libraries(stm32l1xx PUBLIC platform_common)
target_link_libraries(stm32f4xx PUBLIC arch)

target_compile_definitions(stm32l1xx PUBLIC
        -DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY})

# TODO: check that configuration values are defined
target_compile_definitions(stm32l1xx PUBLIC
        -D${ST_DEVICE}
        -DHSE_VALUE=${CONFIG_PLATFORM_HSE_VAL}
        -DCONFIG_IRQ_COUNT=${TARGET_MCU_IRQ_COUNT})

# Pick console driver
message(STATUS "STM32L1XX: Checking [CONFIG_USE_CONSOLE]...")
if(${CONFIG_USE_CONSOLE})
    # TODO: #83 - config name must be reviewed
    if(NOT DEFINED CONFIG_CONSOLE_DEVICE_NUM)
        message(FATAL_ERROR "CONFIG_CONSOLE_DEVICE_NUM must be set in order to use console")
    endif()

    if(NOT DEFINED CONFIG_CONSOLE_BAUD)
        message(FATAL_ERROR "CONFIG_CONSOLE_BAUD must be set in order to use console")
    endif()

    message(STATUS "Console device:  ${CONFIG_CONSOLE_DEVICE_NUM}")
    message(STATUS "Console baud rate: ${CONFIG_CONSOLE_BAUD}")

    configure_file(templates/console.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/generated/platform/console.hpp)
    target_include_directories(stm32l1xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated//)

    # Helps to determine if bypass driver must be enabled and inited
    target_compile_definitions(stm32l1xx PRIVATE -DCONFIG_BYPASS_CONSOLE_ENABLED)
    # Bypass console implementation
    target_sources(stm32l1xx PRIVATE console.cpp)
    message(STATUS "Bypass console is supported and enabled by the platform")
else()
    configure_file(templates/console_stub.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/generated/platform/console.hpp)
    target_include_directories(stm32l1xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated//)
endif()
