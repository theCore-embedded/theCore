add_library(host INTERFACE)
target_include_directories(host INTERFACE export)

add_library(startup INTERFACE)
target_link_libraries(startup INTERFACE main)
