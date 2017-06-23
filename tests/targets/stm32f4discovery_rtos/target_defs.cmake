include(${MCUS_DIR}/stm32f407/mcu_defs.cmake)

# Configuration for current target
theCore_set_target_config(${CMAKE_CURRENT_LIST_DIR}/target_defs.json)

# Enable FreeRTOS
theCore_enable_os(freertos)
theCore_freertos_header_config_dir(${CMAKE_CURRENT_LIST_DIR})
