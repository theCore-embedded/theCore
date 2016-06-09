add_library(libcpp STATIC arm_eabi.cpp streams.cpp)
target_include_directories(libcpp PUBLIC export)

message(STATUS "CORE: Checking [CONFIG_USE_CONSOLE]...")
# Generate stub
if(${CONFIG_USE_CONSOLE})
	message(STATUS "Console is enabled.")
	# Console is enabled and driver is provided by the platform.
	# Work here is to create aliases of bus/bus_pipe classes tied with console.
	target_link_libraries(libcpp PUBLIC ${PLATFORM_NAME})
	target_compile_definitions(libcpp PUBLIC -DUSE_CONSOLE=1)
else()
	message(STATUS "Console is not enabled.")
endif()

add_cppcheck(libcpp UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
target_link_libraries(libcpp PUBLIC types)
target_link_libraries(libcpp PUBLIC bus)

# Unit testing
add_subdirectory(tests)
