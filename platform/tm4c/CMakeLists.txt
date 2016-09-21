add_library(tm4c platform.cpp)

include(mcu_cfg.cmake)

target_link_libraries(tm4c PUBLIC arch)
target_link_libraries(tm4c PUBLIC cmsis)

target_include_directories(tm4c PUBLIC export)
