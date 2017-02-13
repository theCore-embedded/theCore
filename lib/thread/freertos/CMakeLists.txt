add_library(thread_impl INTERFACE)
target_sources(thread_impl INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/mutex.cpp
	${CMAKE_CURRENT_LIST_DIR}/semaphore.cpp
	${CMAKE_CURRENT_LIST_DIR}/thread.cpp
	${CMAKE_CURRENT_LIST_DIR}/utils.cpp
	${CMAKE_CURRENT_LIST_DIR}/signal.cpp)

target_include_directories(thread_impl INTERFACE export)
target_link_libraries(thread_impl INTERFACE dbg INTERFACE ${PLATFORM_NAME})

# If kernel is provided by theCore then use it
if(NOT CONFIG_OS_EXTERNAL)
    target_link_libraries(thread_impl INTERFACE freertos)
endif()

# TODO: provide checks for extistence of external FreeRTOS library.
