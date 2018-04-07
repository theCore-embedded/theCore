# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
        msg_info("Architecture module is disabled")
        return()
    endif()

    if(NOT PLATFORM_NAME STREQUAL "host")
        msg_fatal("Architecture is not set, aborting")
    endif()
else()
    msg_fatal("Unsupported architecture: ${TARGET_MCU_ARCH}")
endif()
