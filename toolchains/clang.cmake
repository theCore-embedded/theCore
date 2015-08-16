# our linker requires special script
INCLUDE(CMakeForceCompiler)

# this one is important
set(CMAKE_SYSTEM_NAME Generic)
#this one not so much
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
# there is no LLVM linker that can work with linker script and true
# bare metal (or, maybe, I didn't google'd enough?)
set(CMAKE_C_LINKER arm-none-eabi-ld)
CMAKE_FORCE_C_COMPILER(clang Clang)
CMAKE_FORCE_CXX_COMPILER(clang++ Clang)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH /usr/bin)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)