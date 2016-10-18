include(${MCUS_DIR}/tm4c123gh6pm/mcu_defs.cmake)

# Enable STUB bypass console. Will be changed to the CONFIG_USE_CONSOLE
# when UART driver will be ready
set(CONFIG_USE_BYPASS_CONSOLE 1)
