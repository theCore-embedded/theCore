# This file includes proper listfiles with respect to the compiler being used

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# using Clang
	include(${CMAKE_CURRENT_LIST_DIR}/clang/clang.cmake)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	# using GCC
	include(${CMAKE_CURRENT_LIST_DIR}/gnu/gnu.cmake)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
	# using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
	# using Visual Studio C++
endif()
