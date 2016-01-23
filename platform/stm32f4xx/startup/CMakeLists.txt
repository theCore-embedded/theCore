# TODO: comments
add_library(startup STATIC
start.s
)

# ASM code requires some symbols present in outer libraries
target_link_libraries(startup sys)
