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
add_unit_host_test(NAME memset SOURCES tests/memset_unit.cpp memset.c)

if(${CORE_PROVIDES_LIBC})
    target_include_directories(emc PUBLIC export)
else()
    # Do not expose libc headers from theCore if this is not required.
    target_compile_definitions(emc PRIVATE -DEXTERNAL_LIBC)
    target_include_directories(emc PRIVATE export)
endif()

# Static analysis
add_cppcheck(emc STYLE FORCE)
