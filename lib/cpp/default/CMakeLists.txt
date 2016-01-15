# Dummy console driver using only when it is not provided by the project
add_library(default_console_driver INTERFACE)
target_include_directories(default_console_driver INTERFACE export)
