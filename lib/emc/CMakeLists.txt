# This is a temporary library that suit basic needs
# prior to moment, when newlib will be inlcuded

add_library(emc STATIC
memset.c
memcpy.c
memmove.c
strcmp.c
strchr.c
strlen.c
isupper.c
islower.c
isalpha.c
tolower.c
)

# Tests
add_unit_host_test(test_memset tests/memset_unit.cpp memset.c)
add_unit_host_test(test_memcpy tests/memcpy_unit.cpp memcpy.c)

target_include_directories(emc PUBLIC export)
target_include_directories(emc PRIVATE .)
