if("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32F4")
    set(STM32_FAMILY f4xx)
elseif("${CONFIG_PLATFORM_DEVICE}" MATCHES "STM32L1")
    set(STM32_FAMILY l1xx)
else()
    # Likely that additional implementation is required in order to add new stm32
    # family
    message("Device ${CONFIG_PLATFORM_DEVICE} doesn't belong to any stm32 family")
endif()

# Prevent warnings in sources provided by ST
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")

# Grab some MCU definitions
include(${STM32_FAMILY}/mcu_cfg.cmake)

add_library(stm32_family ${STM32_FAMILY}/system_stm32${STM32_FAMILY}.c)

target_compile_definitions(stm32_family PUBLIC
        -D${ST_DEVICE}
        -DHSE_VALUE=${CONFIG_PLATFORM_HSE_VAL}
        -DCONFIG_IRQ_COUNT=${TARGET_MCU_IRQ_COUNT}
        )

target_include_directories(stm32_family PUBLIC ${STM32_FAMILY}/export)
target_link_libraries(stm32_family PUBLIC cmsis)
target_link_libraries(stm32_family PUBLIC stm32_spl)
