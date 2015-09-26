# TODO: comments
add_library(startup STATIC
utils.c
start.s
)

# ASM code requires some symbols present in outer libraries
target_link_libraries(startup sys)
target_link_libraries(startup platform)
target_link_libraries(startup target)
