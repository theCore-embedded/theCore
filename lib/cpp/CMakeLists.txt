## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS *.hpp)
add_custom_target(libcpp_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(libcpp STATIC arm_eabi.cpp)
target_include_directories(libcpp PUBLIC export)

message(STATUS "Determining console driver configuration...")
# Pick console driver
if (DEFINED CONSOLE_CONFIG_LIB)
	if (TARGET "${CONSOLE_CONFIG_LIB}")
		message(STATUS "Console library \"${CONSOLE_CONFIG_LIB}\" is provided by the project")
	else()
		message(FATAL_ERROR "${CONSOLE_CONFIG_LIB} is not a valid console library!")
	endif()
else()
	message(STATUS "Console library is not provided by the project, so default is used")
	add_subdirectory(default)
	set(CONSOLE_CONFIG_LIB default_console_driver)
endif()

# Some of the includes are provided by the project itself
target_link_libraries(libcpp PUBLIC ${CONSOLE_CONFIG_LIB})
