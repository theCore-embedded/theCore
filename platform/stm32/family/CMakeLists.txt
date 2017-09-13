set(DMA_CFG_HPP ${CMAKE_CURRENT_BINARY_DIR}/export/family_dma_cfg.hpp)

if(CONFIG_PLATFORM_DEVICE MATCHES "STM32F4")
    set(STM32_FAMILY f4xx)
elseif(CONFIG_PLATFORM_DEVICE MATCHES "STM32L1")
    set(STM32_FAMILY l1xx)
    # TODO: implement generator for L1 family
    unset(DMA_CFG_HPP)
else()
    # Likely that additional implementation is required in order to add new stm32
    # family
    msg_fatal("Device ${CONFIG_PLATFORM_DEVICE} doesn't belong to any stm32 family")
endif()

theCore_create_cog_runner(
    IN      ${CMAKE_CURRENT_LIST_DIR}/${STM32_FAMILY}/templates/dma_cfg.in.hpp
    OUT     ${DMA_CFG_HPP}
    DEPENDS ${THECORE_TARGET_CONFIG_FILE}
    ARGS    "-DJSON_CFG=${THECORE_TARGET_CONFIG_FILE}")

# Prevent warnings in sources provided by ST
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")

# Grab some MCU definitions
include(${STM32_FAMILY}/mcu_cfg.cmake)

add_library(stm32_family
    ${STM32_FAMILY}/system_stm32${STM32_FAMILY}.c
    # Adding auto-generated header will automatically put all dependencies in
    # place.
    ${DMA_CFG_HPP})

# Export family-related definitions.
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/templates/defines.in.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/export/family_defines.hpp
        )

target_include_directories(stm32_family PUBLIC
        ${STM32_FAMILY}/export export ${CMAKE_CURRENT_BINARY_DIR}/export)
target_link_libraries(stm32_family PUBLIC cmsis stm32_spl)
