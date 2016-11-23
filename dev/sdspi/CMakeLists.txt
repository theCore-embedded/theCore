add_library(sdspi INTERFACE)
target_include_directories(sdspi INTERFACE export)
target_link_libraries(sdspi INTERFACE utils)
