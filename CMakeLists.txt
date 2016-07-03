# ASM is crucial
enable_language(ASM-ATT)
# All subproject may use tests
enable_testing()

# Obvious requirement
cmake_minimum_required(VERSION 3.4)

# Our project name
project(the_core)

# C++14 is required
set(CMAKE_CXX_STANDARD 14)

include(build_api.cmake)
include(Findcppcheck)
include(CppcheckTargets)

# TODO: add same configuration mechanism for choosing right kernel
message(STATUS "Checking [CONFIG_PLATFORM]...")
if(NOT DEFINED CONFIG_PLATFORM)
    message(STATUS "CONFIG_PLATFORM is not set, defaulting to 'host'...")
    set(PLATFORM_NAME host)
else()
    message(STATUS "Platform will be used: ${CONFIG_PLATFORM}")
    set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convinience
endif()

# Set general flags for C\C++ compiler and linker
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")

# Linker definitions is propagated by the platform.
include(${CORE_DIR}/platform/common/linker.cmake)

# The top-level library target, essence of this project.
add_library(${PROJECT_NAME} INTERFACE)
# TODO: move this dependency somewhere?
target_link_libraries(${PROJECT_NAME} INTERFACE arch)
target_link_libraries(${PROJECT_NAME} INTERFACE ${PLATFORM_NAME})

# Platform is added in the first place in order to expose platform definitions
# to the rest of the system
add_subdirectory(platform)

# Required layer only when cross-compiling
if(CMAKE_CROSSCOMPILING)
    add_subdirectory(sys)
endif()

# Rest of modules
add_subdirectory(arch)
add_subdirectory(lib)
add_subdirectory(dev)
add_subdirectory(kernel)
add_dependencies(all_cppcheck ${PROJECT_NAME})

# Doc must have access to all ignore doxygen dirs, defined elsewhere.
# Thus it is included after all the others.
add_subdirectory(doc)
