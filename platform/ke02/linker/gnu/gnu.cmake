set(CMAKE_C_LINK_FLAGS
        "-fuse-linker-plugin -march=armv6s-m -mcpu=cortex-m0plus -nostartfiles \
        -specs=nano.specs -mthumb -Wl,--gc-sections"
        CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
