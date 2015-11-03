# Flags and definitions used with GNU complilation suite

# avoid using any additional flags when linking with shared libraries
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

# set various flags for C\C++ compiler and linker
# warn flags goes here
# TODO: separate debug\release builds
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

# optimization flags
set(CC_OPT_FLAGS "-Os -flto=4 ")
set(CXX_OPT_FLAGS "${CC_OPT_FLAGS}")
# used in debug mode
set(CC_NO_OPT_FLAGS "-O0 -g3")
set(CXX_NO_OPT_FLAGS "${CC_NO_OPT_FLAGS}")

# common flags for current platform
set(CC_PLATFORM_FLAGS "-ffreestanding -mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections -fno-common")

# -fno-use-cxa-atexit helps resolve issue with DSO handle undefined reference
# why????
set(CXX_PLATFORM_FLAGS "-fno-use-cxa-atexit -fno-exceptions -fno-rtti ${CC_PLATFORM_FLAGS}")

# extra flags
set(C_CXX_EXTRA_FLAGS "-gdwarf-2 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp ")
set(CC_EXTRA_FLAGS "-std=c99 ${C_CXX_EXTRA_FLAGS}")
set(CXX_EXTRA_FLAGS "-std=c++14 ${C_CXX_EXTRA_FLAGS}")

# Supported modes are normal, release, debug and minimum size
# Normal mode
set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} ${CC_PLATFORM_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} ${CXX_PLATFORM_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")

# Default flags of the release mode are OK for our builds
# Default flags of the minimum size mode are OK for our builds
# Debug mode
set(CMAKE_C_FLAGS_DEBUG  "${CC_OPT_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_OPT_FLAGS}")

# Linker flags
set(CMAKE_C_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -flto -Wl,--gc-sections ")
set(CMAKE_CXX_LINK_FLAGS
	"-fuse-linker-plugin -nostdlib -nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -flto -Wl,--gc-sections ")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_C_LINK_EXECUTABLE
"arm-none-eabi-gcc <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE
"arm-none-eabi-g++ <OBJECTS> <CMAKE_CXX_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")

# TODO: merge these flags with compiler one
set(CMAKE_ASM-ATT_COMPILER_ARG1 "-mcpu=cortex-m4")

