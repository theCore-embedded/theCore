# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# this one is important
set(CMAKE_SYSTEM_NAME Generic)
# this one not so much
set(CMAKE_SYSTEM_VERSION 1)

# GCC is used as a linker in order to use LTO properly
set(CMAKE_C_LINKER arm-none-eabi-gcc)
set(CMAKE_CXX_LINKER arm-none-eabi-g++)

# specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# Way to avoid specifying linking flags in the toolchain.
# See https://cmake.org/pipermail/cmake-developers/2016-February/027888.html
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH /usr/arm-none-eabi)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

################################################################################
# Flags and definitions used with GNU complilation suite

# Essential stuff for using 3rd-party libs
set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

# avoid using any additional flags when linking with shared libraries
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

# common flags for current platform
set(CC_PLATFORM_FLAGS "-specs=nano.specs -ffreestanding -mcpu=cortex-m0plus -mthumb -march=armv6s-m \
    -fdata-sections -ffunction-sections -fno-common")

# -fno-use-cxa-atexit helps resolve issue with DSO handle undefined reference
# why????
set(CXX_PLATFORM_FLAGS "-fno-use-cxa-atexit -fno-exceptions -fno-rtti ${CC_PLATFORM_FLAGS}")

# TODO: move std and gdwarf flags out of toolchain into the core listfile itself
set(C_CXX_EXTRA_FLAGS "-gdwarf-2")
set(CC_EXTRA_FLAGS "-std=c99 ${C_CXX_EXTRA_FLAGS}")
set(CXX_EXTRA_FLAGS "-std=c++14 ${C_CXX_EXTRA_FLAGS}")

# Set general flags for C\C++ compiler and linker
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

# Supported modes are normal, release, debug and minimum size
# Normal mode
set(CMAKE_C_FLAGS
	"${CMAKE_C_FLAGS} ${CC_PLATFORM_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}"
	CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} ${CXX_PLATFORM_FLAGS} ${CXX_WARN_FLAGS} ${CXX_EXTRA_FLAGS}"
	CACHE STRING "C++ flags")

# Release flags, optimization is on,
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG "
	CACHE STRING "Release C flags")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE}
	CACHE STRING "Release C++ flags")

# Minimum size release flags, LTO and minimum size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG "
	CACHE STRING "Minsize C flags")
set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL}
	CACHE STRING "Minsize C++ flags")

# Debug mode, no LTO and maximum debug info
set(CMAKE_C_FLAGS_DEBUG  "-Og -g3 "
	CACHE STRING "Debug C flags")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG}
	CACHE STRING "Debug C++ flags")

set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE STRING "Objcopy executable")

set(CMAKE_ASM-ATT_COMPILE_OBJECT
  "<CMAKE_ASM-ATT_COMPILER> -mthumb -march=armv6s-m -mcpu=cortex-m0plus -o <OBJECT> <SOURCE>")
set(CMAKE_C_LINK_EXECUTABLE
	"${CMAKE_C_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE
	"${CMAKE_CXX_LINKER} <OBJECTS> <CMAKE_CXX_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
