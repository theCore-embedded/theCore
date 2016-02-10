add_library(libcpp STATIC arm_eabi.cpp streams.cpp)
target_include_directories(libcpp PUBLIC export)

message(STATUS "Determining console driver configuration...")
# Pick console driver
if (${CONFIG_USE_CONSOLE})
	if (DEFINED CONFIG_CONSOLE_LIB)
		if (TARGET "${CONFIG_CONSOLE_LIB}")
			message(STATUS "Console library \"${CONFIG_CONSOLE_LIB}\" "
				"is provided by the project")
		else()
			message(FATAL_ERROR "${CONFIG_CONSOLE_LIB} is not a valid console library!")
		endif()
	else()
		message(FATAL_ERROR "Provide a console configuration library"
			"if you want to use console.")
	endif()
else()
	message(STATUS "Console is not enabled.")
	add_subdirectory(default)
	set(CONFIG_CONSOLE_LIB default_console_driver)
endif()

# Some of the includes are provided by the project itself
target_link_libraries(libcpp PUBLIC ${CONFIG_CONSOLE_LIB})
add_cppcheck(libcpp UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
