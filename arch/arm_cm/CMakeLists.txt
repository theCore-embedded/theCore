# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# ARM Cortex-M architecture
add_library(arch STATIC startup_arm_cm.S arch.cpp)

# By default CMake do not pass compile definitions to ASM files.
# Probably due to the fact that it depends on assembler used and some of the
# assemblers may not support preprocessing.
# This command forces use of gcc instead of as. It allows to pass compile definitions
set_source_files_properties(startup_arm_cm.S PROPERTIES LANGUAGE C)

# ASM code requires some symbols present in outer libraries
target_link_libraries(arch PRIVATE sys)
target_compile_definitions(arch PRIVATE
        -D__START=core_main
        -D__STARTUP_CLEAR_BSS
        -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})

target_compile_definitions(arch PUBLIC -D${TARGET_MCU_ARCH})

# Represents default value for the THECORE_CONFIG_SYSTMR_FREQ (if SYSTMR is enabled)
# The value of the THECORE_CONFIG_SYSTMR_FREQ must be in the range [20Hz, 1000Hz]
set(THECORE_DEFAULT_SYSTMR_FREQ 50)

# CMSIS compliant platform must provide device.hpp header, that will be used
# by CMSIS core drivers. Otherwise, architecture modules cannot provide
# any CMSIS-based implementation, such as execution control and a platform
# must implement that functions on its side.
if(NOT TARGET_PLATFORM_IS_NOT_CMSIS_COMPLIANT)
    if(NOT CONFIG_OS)
        if(THECORE_CONFIG_USE_SYSTMR_SYSTICK)
            target_compile_definitions(arch PUBLIC -DTHECORE_OWNS_SYSTMR=1)
        endif()

        if (NOT THECORE_CONFIG_SYSTMR_FREQ)
            set(THECORE_CONFIG_SYSTMR_FREQ ${THECORE_DEFAULT_SYSTMR_FREQ})
        endif()

        target_compile_definitions(arch PUBLIC
            -DPLATFORM_CMSIS_COMPLIANT
            -DTHECORE_ENABLE_SYSTMR_API # Timer API is enabled in any case, even if not used by theCore.
            -DTHECORE_CONFIG_SYSTMR_FREQ=${THECORE_CONFIG_SYSTMR_FREQ}
            -DTHECORE_USE_WFI_WFE=1  # Architecture provides CMSIS-based WFI/WFE routines
        )
    endif()

    target_compile_definitions(arch PUBLIC
            -DPLATFORM_CMSIS_COMPLIANT
            -DTHECORE_USE_WFI_WFE=1  # Architecture provides CMSIS-based WFI/WFE routines
    )

    target_sources(arch PUBLIC ${CMAKE_CURRENT_LIST_DIR}/execution.cpp)
endif()

# Apply memory layout
configure_file(gcc_arm_cm.ld ${CMAKE_CURRENT_BINARY_DIR}/gcc_arm_cm.ld)

# Use linker script
target_link_libraries(arch PUBLIC -T${CMAKE_CURRENT_BINARY_DIR}/gcc_arm_cm.ld)

target_include_directories(arch PUBLIC export)
