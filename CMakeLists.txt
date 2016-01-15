include(CheckIncludeFileCXX)

message(STATUS "Determining project configuration...")

# TODO: add same configuration mechanism for choosing right kernel

# Add platform to the builde
add_subdirectory(${PLATFORM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/platform)

add_subdirectory(sys ${CMAKE_CURRENT_BINARY_DIR}/core/sys)
add_subdirectory(lib ${CMAKE_CURRENT_BINARY_DIR}/core/lib)
add_subdirectory(dev ${CMAKE_CURRENT_BINARY_DIR}/core/dev)
add_subdirectory(kernel ${CMAKE_CURRENT_BINARY_DIR}/core/kernel)
