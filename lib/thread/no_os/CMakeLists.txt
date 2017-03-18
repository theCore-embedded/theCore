add_library(thread_impl INTERFACE)
target_include_directories(thread_impl INTERFACE export)
target_link_libraries(thread_impl INTERFACE types)

target_sources(thread_impl INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mutex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/semaphore.cpp
    ${CMAKE_CURRENT_LIST_DIR}/spinlock.cpp)

# For platforms that support WFI/WFE instructions (or similar) special semaphore
# logic can be implemented for reduced power consumtions during wait periods.
if(TARGET_MCU_WFI_WFE_PRESENT)
    target_compile_definitions(thread_impl PRIVATE -DUSE_WFI_WFE)
endif()

add_cppcheck(thread_impl UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
