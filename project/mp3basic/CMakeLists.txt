cmake_minimum_required(VERSION 2.8)

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(mp3basic_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

# Our project sources
add_executable(${BASE_PROJECT_NAME} main.cpp)

# Register project with target
link_project_with_target()

target_link_libraries(${BASE_PROJECT_NAME} pcd8544)
target_link_libraries(${BASE_PROJECT_NAME} sys)
target_link_libraries(${BASE_PROJECT_NAME} sdspi)
target_link_libraries(${BASE_PROJECT_NAME} fat)
target_link_libraries(${BASE_PROJECT_NAME} allocators)
target_link_libraries(${BASE_PROJECT_NAME} fs)
