add_library(thread_os mutex.cpp future.cpp semaphore.cpp thread.cpp)
target_include_directories(thread_os PUBLIC export)
target_link_libraries(thread_os PUBLIC utils pthread)
