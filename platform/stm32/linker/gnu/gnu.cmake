# Linker flags.
# TODO: check redundancy when using -msoft-float and -mfloat-abi=soft together
# TODO: alternate flags according to choosen family.
set(CMAKE_C_LINK_FLAGS
	"-fuse-linker-plugin -mcpu=cortex-m3 -msoft-float -mfloat-abi=soft \
	-nostartfiles -mthumb -flto -Wl,--gc-sections"
	CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
