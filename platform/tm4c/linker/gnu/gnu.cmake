# Linker flags. TODO: check redundancy when using -msoft-float and -mfloat-abi=soft together
set(CMAKE_C_LINK_FLAGS
        "-fuse-linker-plugin -mcpu=cortex-m4 -msoft-float -mfloat-abi=softfp \
	    -mfpu=fpv4-sp-d16 -nostartfiles -mthumb -flto -Wl,--gc-sections"
        CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
