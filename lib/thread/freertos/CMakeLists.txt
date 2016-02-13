add_library(thread_os os.cpp)
target_include_directories(thread_os PUBLIC .)

# If kernel is provided by us then built it
if (${CONFIG_OS_INTERNAL})
	target_link_libraries(thread_os PRIVATE freertos)
endif()
