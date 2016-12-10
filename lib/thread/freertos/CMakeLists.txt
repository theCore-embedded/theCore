add_library(thread_os INTERFACE)
target_sources(thread_os INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/mutex.cpp
	${CMAKE_CURRENT_LIST_DIR}/semaphore.cpp
	${CMAKE_CURRENT_LIST_DIR}/thread.cpp
	${CMAKE_CURRENT_LIST_DIR}/utils.cpp
	${CMAKE_CURRENT_LIST_DIR}/signal.cpp)

target_include_directories(thread_os INTERFACE  .)
target_include_directories(thread_os INTERFACE export)
target_link_libraries(thread_os INTERFACE thread_common)
target_link_libraries(thread_os INTERFACE dbg)
target_link_libraries(thread_os INTERFACE ${PLATFORM_NAME})

# If kernel is provided by theCore then use it
if(CONFIG_OS_INTERNAL)
	target_link_libraries(thread_os INTERFACE freertos)
endif()

# TODO: provide checks for extistence of external FreeRTOS library.
