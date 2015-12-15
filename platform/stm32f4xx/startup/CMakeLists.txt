# TODO: comments
add_library(startup STATIC
utils.c
start.s
)

# ASM code requires some symbols present in outer libraries
target_link_libraries(startup sys)
target_link_libraries(startup ${BASE_PLATFORM_NAME})
target_link_libraries(startup ${BASE_TARGET_NAME})
target_link_libraries(startup freertos)
