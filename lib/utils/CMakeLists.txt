add_library(utils assert.cpp)
target_include_directories(utils PUBLIC export)
target_link_libraries(utils PRIVATE libcpp)
