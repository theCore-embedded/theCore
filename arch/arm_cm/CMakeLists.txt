# ARM Cortex-M architecture
add_library(arch STATIC startup_arm_cm.S)

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

if(THECORE_USE_NVIC_IRQ_NAMES)
    target_compile_definitions(arch PRIVATE -DTHECORE_USE_NVIC_IRQ_NAMES)
endif()

# Apply memory layout
configure_file(gcc_arm_cm.ld ${CMAKE_CURRENT_BINARY_DIR}/gcc_arm_cm.ld)

# Use linker script
target_link_libraries(arch PUBLIC -T${CMAKE_CURRENT_BINARY_DIR}/gcc_arm_cm.ld)


