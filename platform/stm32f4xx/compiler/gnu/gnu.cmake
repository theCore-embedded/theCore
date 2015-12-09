# Flags and definitions used with GNU complilation suite

# avoid using any additional flags when linking with shared libraries
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

# set various flags for C\C++ compiler and linker
# warn flags goes here
# TODO: separate debug\release builds
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

# common flags for current platform
set(CC_PLATFORM_FLAGS "-ffreestanding -mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections -fno-common")

# -fno-use-cxa-atexit helps resolve issue with DSO handle undefined reference
# why????
set(CXX_PLATFORM_FLAGS "-fno-use-cxa-atexit -fno-exceptions -fno-rtti ${CC_PLATFORM_FLAGS}")

# extra flags, filename macro is included for using it in assertions
set(C_CXX_EXTRA_FLAGS "-gdwarf-2 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fstack-protector-all")
set(CC_EXTRA_FLAGS "-std=c99 ${C_CXX_EXTRA_FLAGS}")
set(CXX_EXTRA_FLAGS "-std=c++14 ${C_CXX_EXTRA_FLAGS}")

# Supported modes are normal, release, debug and minimum size
# Normal mode
set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} ${CC_PLATFORM_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} ${CXX_PLATFORM_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")

# Release flags, optimization is on,
set(CMAKE_C_FLAGS_RELEASE "-O3 -flto=4 -ffat-lto-objects ")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

# Minimum size release flags, LTO and minimum size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -flto=4 -ffat-lto-objects ")
set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})

# Debug mode, no LTO and maximum debug info
set(CMAKE_C_FLAGS_DEBUG  "-O0 -g3 ")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

# GCC is used as a linker in order to use LTO properly
set(CMAKE_C_LINKER "arm-none-eabi-gcc")
set(CMAKE_CXX_LINKER "arm-none-eabi-g++")

# Linker flags
set(CMAKE_C_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -flto -Wl,--gc-sections ")
set(CMAKE_CXX_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -flto -Wl,--gc-sections ")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_C_LINK_EXECUTABLE
"${CMAKE_C_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE
"${CMAKE_CXX_LINKER} <OBJECTS> <CMAKE_CXX_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")

# TODO: merge these flags with compiler one
set(CMAKE_ASM-ATT_COMPILER_ARG1 "-mcpu=cortex-m4")

