set(PLATFORM_DIR ${CORE_DIR}/platform/${PLATFORM_NAME}/)

if(CMAKE_C_COMPILER MATCHES "clang")
    include(${PLATFORM_DIR}linker/clang/clang.cmake)
elseif(CMAKE_C_COMPILER MATCHES "gcc" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    include(${PLATFORM_DIR}linker/gnu/gnu.cmake)
else()
    # If you see this I encourage you to implement such support! :)
    msg_fatal("Compiler: ${CMAKE_C_COMPILER} is not supported!")
endif()
