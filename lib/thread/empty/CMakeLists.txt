add_library(thread mutex.cpp semaphore.cpp future.cpp)
target_include_directories(thread PUBLIC export)
