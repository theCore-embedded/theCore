add_library(libcpp INTERFACE)

if(CMAKE_CROSSCOMPILING)
    target_sources(libcpp INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/arm_eabi.cpp)
endif()

target_sources(libcpp INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/streams.cpp)

target_include_directories(libcpp
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}/export)

message(STATUS "CORE: Checking [CONFIG_USE_CONSOLE]...")
# Generate stub
if(CONFIG_USE_CONSOLE)
    message(STATUS "Console is enabled.")
    # Console is enabled and driver is provided by the platform.
    # Work here is to create aliases of bus/bus_pipe classes tied with console.
    target_link_libraries(libcpp INTERFACE ${PLATFORM_NAME})
    target_compile_definitions(libcpp INTERFACE -DUSE_CONSOLE=1)
    target_link_libraries(libcpp INTERFACE bus)
else()
    message(STATUS "Console is not enabled.")
endif()

add_cppcheck(libcpp UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
target_link_libraries(libcpp INTERFACE types)

add_unit_host_test(NAME istream_test
        SOURCES tests/istream_unit.cpp
        INC_DIRS export)

add_unit_host_test(NAME ostream_test
        SOURCES tests/ostream_unit.cpp
        INC_DIRS export)
