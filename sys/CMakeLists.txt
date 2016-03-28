# TODO: move it to a separate module
add_library(common_io INTERFACE)
target_include_directories(common_io INTERFACE export)

add_library(sys STATIC sys.cpp)

target_link_libraries(sys ${CONFIG_PROJECT_LIB})
# IRQ manager is required that residing in platform
target_link_libraries(sys ${PLATFORM_NAME})
# Requires libcpp to initialize streams
target_link_libraries(sys libcpp)

# A special initialization is required if kernel is present
if (DEFINED CONFIG_OS)
	target_link_libraries(sys ${CONFIG_OS}_main)
else()
	target_sources(sys PRIVATE kernel_stubs.c)
endif()
