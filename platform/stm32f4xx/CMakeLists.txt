# Library provides platform headers, definitiions and code
add_library(stm32f4xx STATIC
        platform.cpp
        irq_manager.cpp
        exti_manager.cpp
        utils.c
        )

target_include_directories(stm32f4xx PUBLIC export)

# Find out particular MCU configuration
include(mcu_cfg.cmake)

# Platform submodules
add_subdirectory(f4)

# TODO: clarify essential libraries
target_link_libraries(stm32f4xx PUBLIC stm32f4xx_spl)
target_link_libraries(stm32f4xx PUBLIC platform_common)
target_link_libraries(stm32f4xx PUBLIC types)
target_link_libraries(stm32f4xx PUBLIC containers)
target_link_libraries(stm32f4xx PUBLIC emc) # For memset
target_link_libraries(stm32f4xx PUBLIC system_stm32f4xx)

add_cppcheck(stm32f4xx)

# Maximum priority of user ISRs can be redefined, # but be careful:
# it heavily depends on RTOS constraits.
# I.e. for FreeRTOS maximum user priorities should be logically
# less or equal than configMAX_SYSCALL_INTERRUPT_PRIORITY
# (numerical priority representation of M4 is such that higher numbers
# corresponds to smaller logical priorities)

message(STATUS "Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
    set(CONFIG_MAX_ISR_PRIORITY 0xff)
    message(STATUS "CONFIG_MAX_ISR_PRIORITY not set,"
            " using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_compile_definitions(stm32f4xx PUBLIC
        -DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY})
target_compile_definitions(stm32f4xx PUBLIC -DCONFIG_IRQ_COUNT=${TARGET_MCU_IRQ_COUNT})

# Pick console driver
message(STATUS "STM32F4XX: Checking [CONFIG_USE_CONSOLE]...")
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
    target_include_directories(stm32f4xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/)

    # Helps to determine if bypass driver must be enabled and inited
    target_compile_definitions(stm32f4xx PRIVATE -DCONFIG_BYPASS_CONSOLE_ENABLED)
    # Bypass console implementation
    target_sources(stm32f4xx PRIVATE console.cpp)
    message(STATUS "Bypass console is supported and enabled by the platform")
else()
    configure_file(templates/console_stub.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/generated/platform/console.hpp)
    target_include_directories(stm32f4xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated/)
endif()
