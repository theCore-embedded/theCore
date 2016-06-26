add_library(startup startup_ARMCM3.c)
target_link_libraries(startup sys)
target_link_libraries(startup stm32l1xx)

target_compile_definitions(startup PRIVATE
        -D__START=core_main
        -D__STARTUP_CLEAR_BSS
        -D__HEAP_SIZE=0)

# TODO: remove this temporary stub
target_compile_options(startup PRIVATE -Wno-pedantic)

