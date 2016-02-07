add_library(thread mutex.cpp semaphore.cpp future.cpp)
target_include_directories(thread PUBLIC export)

# Tests
add_unit_host_test(test_default_semaphore tests/semaphore_unit.cpp semaphore.cpp)
# TODO: proper dependenices! What is about compilation on Win system?
target_link_libraries(test_default_semaphore pthread)
target_include_directories(test_default_semaphore PRIVATE export)
