add_library(thread os.cpp)
target_include_directories(thread INTERFACE .)

# If kernel is provided by us then built it
if (${CONFIG_OS_INTERNAL})
	target_link_libraries(thread PRIVATE freertos)
endif()
