if("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32F4")
    set(STM32_FAMILY f4xx)
elseif("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32L1")
    set(STM32_FAMILY l1xx)
else()
    # Likely that additional implementation is required in order to add new stm32
    # family
    message(FATAL_ERROR "Device ${CONFIG_PLATFORM_DEVICE} doesn't belong to any stm32 family")
endif()

# Prevent warnings in sources provided by ST
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")

# Grab some MCU definitions
include(${STM32_FAMILY}/mcu_cfg.cmake)

add_library(stm32_family ${STM32_FAMILY}/system_stm32${STM32_FAMILY}.c)

target_compile_definitions(stm32_family PUBLIC
        -Dstm32${STM32_FAMILY} # Used by theCore
        -D${ST_DEVICE} # Used by SPL
        -DCONFIG_SPI_COUNT=${TARGET_MCU_SPI_COUNT} # Used by the stm32 platform
        -DCONFIG_ECL_EXTI_DIRECT_COUNT=${TARGET_MCU_EXTI_DIRECT_COUNT} # User by EXTI driver
        -DCONFIG_ECL_EXTI_GROUPED_COUNT=${TARGET_MCU_EXTI_GROUPED_COUNT} # User by EXTI driver
        -DHSE_VALUE=${CONFIG_PLATFORM_HSE_VAL}
        -DCONFIG_IRQ_COUNT=${TARGET_MCU_IRQ_COUNT}
        )

target_include_directories(stm32_family PUBLIC ${STM32_FAMILY}/export export)
target_link_libraries(stm32_family PUBLIC cmsis)
target_link_libraries(stm32_family PUBLIC stm32_spl)
