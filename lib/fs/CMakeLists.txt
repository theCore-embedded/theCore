# TODO: additional check for this type of config
add_library(fs STATIC fs.cpp inode.cpp file_descriptor.cpp dir_descriptor.cpp)
target_include_directories(fs PUBLIC export)
target_link_libraries(fs PUBLIC libcpp dbg utils emc)

add_cppcheck(fs)

add_subdirectory(fat)

