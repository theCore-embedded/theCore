include(${MCUS_DIR}/tm4c123gh6pm/mcu_defs.cmake)

# Enable console for this target.
set(CONFIG_USE_CONSOLE 1)

# UART0 visible trough USB.
set(CONFIG_CONSOLE_DEVICE_NUM 0)
