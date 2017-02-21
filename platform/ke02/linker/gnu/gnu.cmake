set(CMAKE_C_LINK_FLAGS
        "-fuse-linker-plugin -mcpu=cortex-m0plus -nostartfiles -mthumb -Wl,--gc-sections"
        CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
