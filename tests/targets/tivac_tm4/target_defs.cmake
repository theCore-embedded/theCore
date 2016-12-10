include(${MCUS_DIR}/tm4c123gh6pm/mcu_defs.cmake)

# Enable only bypass console for this target to avoid conflicts.
set(CONFIG_USE_BYPASS_CONSOLE 1)

# UART0 visible trough USB.
set(CONFIG_CONSOLE_DEVICE_NUM 0)
