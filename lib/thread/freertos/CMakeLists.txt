add_library(thread_os os.cpp mutex.cpp semaphore.cpp thread.cpp)
target_include_directories(thread_os PUBLIC .)
target_include_directories(thread_os PUBLIC export)
target_link_libraries(thread_os PUBLIC utils)
target_link_libraries(thread_os PUBLIC thread_common)

# If kernel is provided by us then built it
if (${CONFIG_OS_INTERNAL})
	target_link_libraries(thread_os PUBLIC freertos)
endif()
