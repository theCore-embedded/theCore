# `arm_cm*` not an architectures, but ARM cores.
# Still, it is better to threat them as `architectures` of some kind since
# the `core` term is something that peculiar to ARM and collides with
# the project name.

# Check if ARM Cortex-M core is requested
if(TARGET_MCU_ARCH MATCHES "^arm_cm")
    add_subdirectory(arm_cm)
elseif(NOT TARGET_MCU_ARCH) # Empty architecture allowed in some cases
    if(THECORE_FORCE_NO_ARCH)
        # Architecture definitions and functionality is provided elsewhere.
        # This is often a case if the platform uses some weird kind of linker
        # scripts, custom startup code etc.
        message(STATUS "Architecture module is disabled")
        return()
    endif()

    if(NOT PLATFORM_NAME STREQUAL "host")
        message(FATAL_ERROR "Architecture is not set, aborting")
    endif()
else()
    message(FATAL_ERROR "Unsupported architecture: ${TARGET_MCU_ARCH}")
endif()
