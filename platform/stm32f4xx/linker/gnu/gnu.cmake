# Linker flags
set(CMAKE_C_LINK_FLAGS
	"-fuse-linker-plugin -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mcpu=cortex-m4 \
	-nostartfiles -mthumb -flto -Wl,--gc-sections"
	CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags")
