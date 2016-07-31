# TODO: additional check for this type of config
add_library(fs STATIC fs.cpp inode.cpp file_descriptor.cpp dir_descriptor.cpp)
target_include_directories(fs PUBLIC export)
target_link_libraries(fs PUBLIC libcpp)
target_link_libraries(fs PUBLIC common_io)
target_link_libraries(fs PUBLIC dbg)
target_link_libraries(fs PUBLIC utils)
target_link_libraries(fs PUBLIC emc)
add_cppcheck(fs)

add_subdirectory(fat)

