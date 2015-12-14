# This is a temporary library that suit basic needs
# prior to moment, when newlib will be inlcuded

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.h)
add_custom_target(emc_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

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
target_include_directories(emc PUBLIC export)
