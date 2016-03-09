add_subdirectory(cpp)
# Embedded C library. Name was choosed to prevent collisions 
# with standart C library (libc)
add_subdirectory(emc)
add_subdirectory(allocators)
add_subdirectory(fs)
add_subdirectory(utils)
add_subdirectory(types)
add_subdirectory(thread)
