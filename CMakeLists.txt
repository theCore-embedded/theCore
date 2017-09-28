# ASM is crucial
enable_language(ASM-ATT)
# Subproject may use tests
enable_testing()

# Obvious requirement
cmake_minimum_required(VERSION 3.4)

# Our project name
project(the_core LANGUAGES C CXX)

include(build_api.cmake)
include(Findcppcheck)
include(CppcheckTargets)
include(JSONParser)

# Check if theCore should be placed in the test mode.
# Benefit here is supporting of CMake invocation without
# THECORE_TARGET_CONFIG_FILE. It will reduce hassle when running unit tests,
# dropping unnecessary variable that not used in tests anyway.
if(NOT DEFINED THECORE_TARGET_CONFIG_FILE AND NOT CMAKE_CROSSCOMPILING)
    msg_info("Enabled test mode for theCore")

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test.json
        "{\"platform\":{\"name\":\"host\",\"console\":true}}"
    )
    set(THECORE_TARGET_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/test.json"
        CACHE PATH "Test JSON file")
endif()

if(NOT EXISTS ${THECORE_TARGET_CONFIG_FILE})
    msg_warn("Invalid JSON config file: ${THECORE_TARGET_CONFIG_FILE}")
else()
    file(READ ${THECORE_TARGET_CONFIG_FILE} THECORE_JSON_BODY)
    # Lower case for better readability.
    sbeParseJson(thecore_cfg THECORE_JSON_BODY)
    set(CONFIG_PLATFORM ${thecore_cfg.platform.name})
endif()

msg_trace("Checking [CONFIG_PLATFORM]: ${CONFIG_PLATFORM}")

if(NOT CMAKE_CROSSCOMPILING)
    if(DEFINED CONFIG_PLATFORM)
        if(NOT CONFIG_PLATFORM STREQUAL "host")
            msg_fatal("Cross-compiler is required for platform other than 'host'.")
        endif()
    endif()

    # C++14 is required
    set(CMAKE_CXX_STANDARD 14)
else()
    # When cross-compiling, platform must be set explicitly.
    if(NOT DEFINED CONFIG_PLATFORM)
        msg_fatal("CONFIG_PLATFORM must be set when cross-compiling")
    endif()
endif()

msg_trace("Platform will be used: ${CONFIG_PLATFORM}")

set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convenience

# Linker definitions is propagated by the platform.
include(${CORE_DIR}/platform/common/linker.cmake)

# The top-level library target, essence of this project.
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${PLATFORM_NAME} core_cpp)

# Platform is added in the first place in order to expose platform definitions
# to the rest of the system
add_subdirectory(platform)

# Required layer only when cross-compiling
if(CMAKE_CROSSCOMPILING)
    if(NOT THECORE_FORCE_NO_SYS)
        add_subdirectory(sys)
    else()
        # It is possible that other modules, like platform libraries,
        # already provide functionality similar to the sys.
        msg_info("System module is disabled")
    endif()

    if(NOT THECORE_FORCE_NO_NEWLIB)
        # Newlib is theCore default C library (with some limitations).
        target_link_libraries(${PROJECT_NAME} INTERFACE newlib_stubs)
    endif()
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
