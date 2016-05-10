add_library(thread_common mutex.cpp semaphore.cpp spinlock.cpp)
target_include_directories(thread_common PUBLIC export)
target_link_libraries(thread_common PUBLIC types)

add_cppcheck(thread_common UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
