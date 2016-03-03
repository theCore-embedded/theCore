add_library(cmsis st/system_stm32f4xx.c)
target_include_directories(cmsis SYSTEM PUBLIC includes)

# TODO: check if variables are defined
target_compile_definitions(cmsis PUBLIC "-D${CONFIG_PLATFORM_DEVICE}")
target_compile_definitions(cmsis PUBLIC "-DHSE_VALUE=${CONFIG_PLATFORM_HSE_VAL}")
