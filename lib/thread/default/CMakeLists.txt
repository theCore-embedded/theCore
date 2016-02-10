add_library(thread mutex.cpp semaphore.cpp future.cpp)
target_include_directories(thread PUBLIC export)

add_cppcheck(thread UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
