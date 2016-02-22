# Linker flags
set(CMAKE_C_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld \
	-flto -Wl,--gc-sections "
	CACHE STRING "Linker C flags")
set(CMAKE_CXX_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld \
	-flto -Wl,--gc-sections "
	CACHE STRING "Linker C++ flags")
