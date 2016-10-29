add_library(tm4c platform.cpp)

include(mcu_cfg.cmake)

target_link_libraries(tm4c PUBLIC arch cmsis tivaware platform_common)

target_include_directories(tm4c PUBLIC export)

message(STATUS "TM4C: Checking STUB [CONFIG_USE_BYPASS_CONSOLE]...")
if(CONFIG_USE_BYPASS_CONSOLE)
    message(STATUS "TM4C: Bypass console is enabled")
    target_compile_definitions(tm4c PUBLIC -DCONFIG_BYPASS_CONSOLE_ENABLED)
    target_sources(tm4c PRIVATE console.cpp)
endif()
