add_library(host console.cpp)
add_library(startup INTERFACE)

target_include_directories(host PUBLIC export)
target_include_directories(host PUBLIC export/platform)
target_link_libraries(host PUBLIC types)
target_link_libraries(host PUBLIC platform_common)
