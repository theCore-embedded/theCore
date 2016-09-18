# TODO: Issue #138 - GCC 6.2 fails to compile thread.cpp module
add_compile_options(-Wno-error=strict-aliasing)

add_library(thread_os mutex.cpp semaphore.cpp thread.cpp)
target_include_directories(thread_os PUBLIC export)
target_link_libraries(thread_os PUBLIC utils types dbg pthread)
