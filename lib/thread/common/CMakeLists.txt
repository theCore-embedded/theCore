add_library(thread_common future.cpp mutex.cpp semaphore.cpp spinlock.cpp)
target_include_directories(thread_common PUBLIC export)

add_cppcheck(thread_common UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
