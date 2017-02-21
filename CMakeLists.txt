# ASM is crucial
enable_language(ASM-ATT)

# Obvious requirement
cmake_minimum_required(VERSION 3.4)

# Our project name
project(the_core LANGUAGES C CXX)

include(build_api.cmake)
include(Findcppcheck)
include(CppcheckTargets)

# TODO: add same configuration mechanism for choosing right kernel

message(STATUS "Checking [CONFIG_PLATFORM]: ${CONFIG_PLATFORM}")

if(NOT CMAKE_CROSSCOMPILING)
    if(DEFINED CONFIG_PLATFORM)
        if(NOT CONFIG_PLATFORM STREQUAL "host")
            message(FATAL_ERROR "Cross-compiler is required for platform other than 'host'.")
        endif()
    else()
        # It is fine to omit platform name if not cross-compiling
        message(STATUS "CONFIG_PLATFORM not set, defauluting to 'host'...")
        set(CONFIG_PLATFORM "host")
    endif()

    # C++14 is required
    set(CMAKE_CXX_STANDARD 14)
else()
    # When cross-compiling, platform must be set explicitly.
    if(NOT DEFINED CONFIG_PLATFORM)
        message(FATAL_ERROR "CONFIG_PLATFORM must be set when cross-compiling")
    endif()
endif()

message(STATUS "Platform will be used: ${CONFIG_PLATFORM}")

set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convinience

# Linker definitions is propagated by the platform.
include(${CORE_DIR}/platform/common/linker.cmake)

# The top-level library target, essence of this project.
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${PLATFORM_NAME})

# Extra libraries, not enabled by default. Check corresponding libraries
# for enabling switches.
target_link_libraries(${PROJECT_NAME} INTERFACE core_extra_libs)

# Platform is added in the first place in order to expose platform definitions
# to the rest of the system
add_subdirectory(platform)

# Required layer only when cross-compiling
if(CMAKE_CROSSCOMPILING)
    add_subdirectory(sys)

    # Newlib is theCore default C library (with some limitations).
    target_link_libraries(${PROJECT_NAME} INTERFACE newlib_stubs)
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
