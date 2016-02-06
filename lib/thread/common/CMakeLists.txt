add_library(thread_common lock_guard.cpp spinlock.cpp)
target_include_directories(thread_common PUBLIC export)
