## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.h)
add_custom_target(cmsis_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(cmsis ./st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC ./includes)

# TODO: move this two to the target
# Make sure appropriate MCU type is chosen
target_link_libraries(cmsis ${BASE_TARGET_NAME})


