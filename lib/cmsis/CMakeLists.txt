add_library(cmsis INTERFACE)

# Patch resolving issue for clang compiler must be applied.
# CMSIS includes must be copied and then patched.

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMSIS/CMSIS/Include
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/clang-cmsis.patch
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

target_include_directories(cmsis SYSTEM INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/Include)
execute_process(COMMAND "patch"  "-p2"  "-i"  "clang-cmsis.patch"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_VARIABLE PATCH_OUTPUT
    ERROR_VARIABLE PATCH_OUTPUT)

message("CMSIS patch: ${PATCH_OUTPUT}")
