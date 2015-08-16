add_library(platform platform.cpp)
add_subdirectory(startup)
target_link_libraries(platform startup)