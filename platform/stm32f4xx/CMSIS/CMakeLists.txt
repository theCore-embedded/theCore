## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.h)
add_custom_target(cmsis_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(cmsis ./st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC ./includes)
# Make sure appropriate MCU type is chosen
target_compile_definitions(cmsis PUBLIC -DSTM32F40_41xxx)


