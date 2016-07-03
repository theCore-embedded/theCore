add_library(startup startup_ARMCM3.S)
target_link_libraries(startup sys)
target_link_libraries(startup stm32l1xx)

# By default CMake do not pass compile definitions to ASM files.
# Probably due to the fact that
# This command forces use of gcc instead of as. It allows to pass compile definitions
set_source_files_properties(startup_ARMCM3.S PROPERTIES LANGUAGE C)

target_compile_definitions(startup PRIVATE
        -D__START=core_main
        -D__STARTUP_CLEAR_BSS
        -D__HEAP_SIZE=0)

# TODO: remove this temporary stub
#target_compile_options(startup PRIVATE -Wno-pedantic)

