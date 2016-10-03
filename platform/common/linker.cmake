set(PLATFORM_DIR ${CORE_DIR}/platform/${PLATFORM_NAME}/)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# using Clang
	include(${PLATFORM_DIR}linker/clang/clang.cmake)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	# using GCC
	include(${PLATFORM_DIR}linker/gnu/gnu.cmake)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
	# using Intel C++
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# using Visual Studio C++
	# If you see this I encourage you to implement such support! :)
	message(FATAL_ERROR "MSVC not yet supported!")
endif()
