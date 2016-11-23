set(PLATFORM_DIR ${CORE_DIR}/platform/${PLATFORM_NAME}/)

if(CMAKE_C_COMPILER MATCHES "clang")
	# using Clang
	include(${PLATFORM_DIR}linker/clang/clang.cmake)
elseif(CMAKE_C_COMPILER MATCHES "gcc")
	# using GCC
	include(${PLATFORM_DIR}linker/gnu/gnu.cmake)
else()
	# If you see this I encourage you to implement such support! :)
	message(FATAL_ERROR "Compiler: ${CMAKE_C_COMPILER} is not supported!")
endif()
