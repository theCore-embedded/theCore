## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.h)
add_custom_target(cmsis_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(cmsis ./st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC ./includes)

# TODO: check if variables are defined
target_compile_definitions(cmsis PUBLIC "-D${PLATFORM_CONFIG_DEVICE}")
target_compile_definitions(cmsis PUBLIC "-DHSE_VALUE=${PLATFORM_CONFIG_HSE_VAL}")
