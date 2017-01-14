add_library(core_cpp INTERFACE)
target_sources(core_cpp INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/arm_eabi.cpp
        ${CMAKE_CURRENT_LIST_DIR}/streams.cpp)

target_include_directories(core_cpp
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}/export)

message(STATUS "CORE: Checking [CONFIG_USE_CONSOLE]...")
# Generate stub
if(${CONFIG_USE_CONSOLE})
    message(STATUS "Console is enabled.")
    # Console is enabled and driver is provided by the platform.
    # Work here is to create aliases of bus/bus_pipe classes tied with console.
    target_link_libraries(core_cpp INTERFACE ${PLATFORM_NAME})
    target_compile_definitions(core_cpp INTERFACE -DUSE_CONSOLE=1)
else()
    message(STATUS "Console is not enabled.")
endif()

add_cppcheck(core_cpp UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
target_link_libraries(core_cpp INTERFACE types bus)

add_unit_host_test(NAME istream_test
        SOURCES tests/istream_unit.cpp
        INC_DIRS export)

add_unit_host_test(NAME ostream_test
        SOURCES tests/ostream_unit.cpp
        INC_DIRS export)
