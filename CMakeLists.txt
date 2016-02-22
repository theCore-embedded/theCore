# ASM is crucial
enable_language(ASM-ATT)
# All subproject may use tests
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

include(Findcppcheck)
include(CppcheckTargets)

# TODO: add same configuration mechanism for choosing right kernel
message(STATUS "Checking [CONFIG_PLATFORM]...")
if (NOT DEFINED CONFIG_PLATFORM)
	message(FATAL_ERROR "CONFIG_PLATFORM must be set in order to use valid platform")
else()
	message(STATUS "Platform will be used: ${CONFIG_PLATFORM}")
	set(PLATFORM_NAME ${CONFIG_PLATFORM}) # For convinience
endif()

# This is the top-level target, placed here to control required build order.
# It depends on startup module, which is actual entry point (not main module).
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/null.cpp "")
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/null.cpp)
target_link_libraries(${PROJECT_NAME} startup)

# Set general flags for C\C++ compiler and linker
set(CC_WARN_FLAGS "-Wall -Wextra -Wpedantic -Werror")
set(CXX_WARN_FLAGS "${CC_WARN_FLAGS} -Weffc++")

set(C_CXX_EXTRA_FLAGS "-gdwarf-2")
set(CC_EXTRA_FLAGS "-std=c99 ${C_CXX_EXTRA_FLAGS}")
set(CXX_EXTRA_FLAGS "-std=c++14 ${C_CXX_EXTRA_FLAGS}")

set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} ${CC_WARN_FLAGS} ${CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} ${CC_WARN_FLAGS} ${CXX_EXTRA_FLAGS}")

# Clean binary on 'make clean' call
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_NAME}.bin)

# In order to pick proper definitions from the platform it is required
# that platform will be added in first place
add_subdirectory(platform)

# TODO: support binary stripping only for projects that requires its
# Means we are crosscompiling
if (NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL ${CMAKE_SYSTEM_NAME})
	# Make binary from the project object file
	add_custom_target(${PROJECT_NAME}.bin ALL
		COMMAND ${CMAKE_OBJCOPY} --output-format=binary
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_NAME}.bin
		DEPENDS ${PROJECT_NAME}
		COMMENT "Making binary ${PROJECT_NAME}"
	)

	add_subdirectory(sys)
endif()

# Rest of modules
add_subdirectory(lib)
add_subdirectory(dev)
add_subdirectory(kernel)

add_dependencies(all_cppcheck ${PROJECT_NAME})
