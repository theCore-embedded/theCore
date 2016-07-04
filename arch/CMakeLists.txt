# `arm_cm*` not an architectures, but ARM cores.
# Still, it is better to threat them as `architecures` of some kind since
# the `core` term is something that peculiar to ARM and collides with
# the project name.

# Check if ARM Cortex-M core is requested
if("${TARGET_MCU_ARCH}" MATCHES "^arm_cm")
    add_subdirectory(arm_cm)
elseif(NOT "${PLATFORM_NAME}" STREQUAL "host")
    message(FATAL_ERROR "Architecture is not supported: ${TARGET_MCU_ARCH}")
endif()
