add_library(stm32l1xx
        platform.cpp
        console.cpp
        irq_manager.cpp)
add_subdirectory(startup)

message(STATUS "Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
    set(CONFIG_MAX_ISR_PRIORITY 0xff)
    message(STATUS "CONFIG_MAX_ISR_PRIORITY not set,"
            " using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_compile_definitions(stm32l1xx PUBLIC
        -DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY}
        -DCONFIG_IRQ_COUNT=53) # TODO: differntiate it for other devices

target_include_directories(stm32l1xx PUBLIC export)
target_link_libraries(stm32l1xx PUBLIC cmsis)
target_link_libraries(stm32l1xx PUBLIC types)
target_link_libraries(stm32l1xx PUBLIC stm32l1xx_spl)

target_compile_definitions(stm32l1xx PUBLIC -DSTM32L1XX_XL) # TODO: temporary solution

