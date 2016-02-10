# TODO: additional check for this type of config
if (DEFINED CONFIG_FS)
	add_subdirectory(${CONFIG_FS})

	add_library(fs STATIC fs.cpp inode.cpp file_descriptor.cpp dir_descriptor.cpp)
	target_include_directories(fs PUBLIC export)
	target_link_libraries(fs PUBLIC libcpp)
	target_link_libraries(fs PUBLIC common_io)
endif()
