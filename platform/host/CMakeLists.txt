add_library(host INTERFACE)
add_library(startup INTERFACE)
target_link_libraries(startup INTERFACE main)
