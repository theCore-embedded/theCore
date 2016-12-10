add_library(types INTERFACE)
target_sources(types INTERFACE ${CMAKE_CURRENT_LIST_DIR}/err.cpp)
target_include_directories(types INTERFACE export)
