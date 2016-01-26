# set various flags for C\C++ compiler and linker
# warn flags goes here
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

# extra flags, filename macro is included for using it in assertions
set(C_CXX_EXTRA_FLAGS "-gdwarf-2")
set(CC_EXTRA_FLAGS "-std=c99 ${C_CXX_EXTRA_FLAGS}")
set(CXX_EXTRA_FLAGS "-std=c++14 ${C_CXX_EXTRA_FLAGS}")

# Supported modes are normal, release, debug and minimum size
# Normal mode
set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")

# Release flags, optimization is on,
set(CMAKE_C_FLAGS_RELEASE "-O3 ")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

# Minimum size release flags, LTO and minimum size
set(CMAKE_C_FLAGS_MINSIZEREL "-Os ")
set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})

# Debug mode, no LTO and maximum debug info
set(CMAKE_C_FLAGS_DEBUG  "-O0 -g3 ")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
