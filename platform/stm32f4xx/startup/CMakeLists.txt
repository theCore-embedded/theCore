# TODO: comments
add_library(startup STATIC startup_ARMCM4.S)

# By default CMake do not pass compile definitions to ASM files.
# Probably due to the fact that
# This command forces use of gcc instead of as. It allows to pass compile definitions
set_source_files_properties(startup_ARMCM4.S PROPERTIES LANGUAGE C)

# ASM code requires some symbols present in outer libraries
target_link_libraries(startup sys)
#target_compile_definitions(startup PRIVATE -D__HEAP_SIZE=0)
target_compile_definitions(startup PRIVATE
        -D__START=core_main
        -D__STARTUP_CLEAR_BSS)
