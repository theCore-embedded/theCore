add_library(host INTERFACE)
add_library(startup INTERFACE)

target_include_directories(host INTERFACE export)
target_include_directories(host INTERFACE export/platform)
target_link_libraries(host INTERFACE types)
target_link_libraries(host INTERFACE common_bus)
