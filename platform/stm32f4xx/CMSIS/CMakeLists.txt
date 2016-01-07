## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.h)
add_custom_target(cmsis_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(cmsis ./st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC ./includes)
target_link_libraries(cmsis ${PROJECT_CONFIG_LIB})


