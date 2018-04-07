# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Don't build if not used
if(NOT TARGET_MCU_ARCH MATCHES "^arm_cm")
    return()
endif()

add_library(cmsis INTERFACE)

# v4.5.0-8-g0156c6f
theCore_get_thirdparty(CMSIS https://github.com/ARM-software/CMSIS.git 0156c6f40e)
set(CMSIS_DIR ${THECORE_BUILD_THIRDPARTY_DIR}/CMSIS)

# TODO: execute patch iff it was not previously applied.
# This will save compilation time when clang is used.
if(CMAKE_C_COMPILER MATCHES "clang")
    # Patch resolving issue for clang compiler must be applied.
    # CMSIS includes must be copied and then patched.

    file(COPY ${CMSIS_DIR}/CMSIS/Include
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
    )

    file(COPY ${CMAKE_CURRENT_LIST_DIR}/clang-cmsis.patch
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
    )

    target_include_directories(cmsis SYSTEM INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/Include)
    execute_process(COMMAND "patch"  "-p2"  "-i"  "clang-cmsis.patch"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_VARIABLE PATCH_OUTPUT
        ERROR_VARIABLE PATCH_OUTPUT)

    msg_info("CMSIS patch: ${PATCH_OUTPUT}")
else()
    target_include_directories(cmsis SYSTEM INTERFACE ${CMSIS_DIR}/CMSIS/Include)
endif()
