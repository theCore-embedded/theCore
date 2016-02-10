# TODO: review it
# our linker requires special script
INCLUDE(CMakeForceCompiler)

# this one is important
set(CMAKE_SYSTEM_NAME Generic)
# this one not so much
set(CMAKE_SYSTEM_VERSION 1)

# GCC is used as a linker in order to use LTO properly
set(CMAKE_C_LINKER arm-none-eabi-gcc)
set(CMAKE_CXX_LINKER arm-none-eabi-g++)

# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH /usr/arm-none-eabi)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
