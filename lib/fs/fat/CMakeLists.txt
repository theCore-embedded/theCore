## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp *.h)
add_custom_target(fat_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(fat STATIC
fat.cpp
dir.cpp
file.cpp
dir_inode.cpp
file_inode.cpp
pff/src/diskio.c
pff/src/pff.c
pff/src/diskio.c
)

target_include_directories(fat PUBLIC export)
target_include_directories(fat PUBLIC pff)
target_link_libraries(fat fs)
target_link_libraries(fat allocators)
