# TODO: use it as general utility rather than platform-dependent file
# This file includes proper listfiles with respect to the compiler being used

set(PLATFORM_DIR ${CORE_DIR}/platform/${PLATFORM_NAME}/)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# using Clang
	include(${PLATFORM_DIR}compiler/clang/clang.cmake)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	# using GCC
	include(${PLATFORM_DIR}compiler/gnu/gnu.cmake)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
	# using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
	# using Visual Studio C++
endif()
