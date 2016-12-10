add_library(thread_common INTERFACE)
target_include_directories(thread_common INTERFACE export)
target_link_libraries(thread_common INTERFACE types)

target_sources(thread_common INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/mutex.cpp
	${CMAKE_CURRENT_LIST_DIR}/semaphore.cpp
	${CMAKE_CURRENT_LIST_DIR}/spinlock.cpp)

add_cppcheck(thread_common UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
