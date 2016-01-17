# TODO: comments
add_library(startup STATIC
utils.c
start.s
)

# ASM code requires some symbols present in outer libraries
target_link_libraries(startup ${PLATFORM_NAME})
