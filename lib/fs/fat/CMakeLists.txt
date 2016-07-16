add_library(fat STATIC
dir.cpp
file.cpp
dir_inode.cpp
file_inode.cpp
path.cpp
pff/src/diskio.c
pff/src/pff.c
pff/src/diskio.c
)

target_include_directories(fat PUBLIC export)
target_include_directories(fat PUBLIC pff)
target_link_libraries(fat fs)
target_link_libraries(fat allocators)
