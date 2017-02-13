add_library(thread_impl INTERFACE)
target_include_directories(thread_impl INTERFACE export)
target_link_libraries(thread_impl INTERFACE types)

target_sources(thread_impl INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mutex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/semaphore.cpp
    ${CMAKE_CURRENT_LIST_DIR}/spinlock.cpp)

add_cppcheck(thread_impl UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
