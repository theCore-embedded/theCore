# Flags and definitions used with GNU complilation suite

# avoid using any additional flags when linking with shared libraries
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

# set various flags for C\C++ compiler and linker
# warn flags goes here
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

# optimization flags
set(CC_OPT_FLAGS "-O2")
set(CXX_OPT_FLAGS "${CC_OPT_FLAGS}")

# common flags for current platform
set(CC_PLATFORM_FLAGS "-mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections -fno-common")
set(CXX_PLATFORM_FLAGS "-fno-exceptions -fno-rtti ${CC_PLATFORM_FLAGS}")

# extra flags
set(CC_EXTRA_FLAGS "-std=c99 -gdwarf-2")
set(CXX_EXTRA_FLAGS "-std=c++14 -gdwarf-2")

# altogether
set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} ${CC_PLATFORM_FLAGS} ${CC_OPT_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} ${CXX_PLATFORM_FLAGS} ${CXX_OPT_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")
set(CMAKE_C_LINK_FLAGS
"-T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -nostartfiles --gc-sections")
set(CMAKE_CXX_LINK_FLAGS
"-T${CMAKE_CURRENT_LIST_DIR}/stm32.ld -nostartfiles --gc-sections")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_C_LINK_EXECUTABLE
"${CMAKE_C_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE
"${CMAKE_CXX_LINKER} <OBJECTS> <CMAKE_CXX_LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
