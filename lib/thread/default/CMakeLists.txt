add_library(thread_os INTERFACE)
target_include_directories(thread_os INTERFACE export)
target_link_libraries(thread_os INTERFACE thread_common)
target_link_libraries(thread_os INTERFACE ${PLATFORM_NAME})
