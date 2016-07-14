add_library(thread_os mutex.cpp semaphore.cpp thread.cpp utils.cpp signal.cpp)
target_include_directories(thread_os PUBLIC .)
target_include_directories(thread_os PUBLIC export)
target_link_libraries(thread_os PUBLIC thread_common)
target_link_libraries(thread_os PRIVATE dbg)
target_link_libraries(thread_os PRIVATE ${PLATFORM_NAME})

# If kernel is provided by us then built it
if (${CONFIG_OS_INTERNAL})
	target_link_libraries(thread_os PUBLIC freertos)
endif()
