# Linker flags.
# TODO: check redundancy when using -msoft-float and -mfloat-abi=soft together

set(STM32_DEVICE "${thecore_cfg.device}")

if(STM32_DEVICE MATCHES "STM32F4")
    set(FAMILY_CORE "-mcpu=cortex-m4")
elseif(STM32_DEVICE MATCHES "STM32L1")
    set(FAMILY_CORE "-mcpu=cortex-m3")
else()
    # Likely that additional implementation is required in order to add new stm32
    # family
    message(FATAL_ERROR "Device ${STM32_DEVICE} doesn't have any linker flags defined")
endif()

# TODO: alter FPU flags based on device used

set(CMAKE_C_LINK_FLAGS
    "-fuse-linker-plugin ${FAMILY_CORE} -mfpu=fpv4-sp-d16 -mfloat-abi=softfp \
    -nostartfiles -nostdlib -mthumb -flto -Wl,--gc-sections"
	CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")

unset(STM32_DEVICE)