add_library(thread mutex.cpp future.cpp semaphore.cpp)
target_include_directories(thread PUBLIC export)
