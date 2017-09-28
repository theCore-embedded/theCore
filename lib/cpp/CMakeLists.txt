add_library(core_cpp INTERFACE)

if(CMAKE_CROSSCOMPILING)
    target_sources(core_cpp INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/arm_eabi.cpp)
endif()

target_sources(core_cpp INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/streams.cpp)

target_include_directories(core_cpp
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}/export)

msg_trace("CORE: Checking [CONFIG_USE_CONSOLE]...")

# Generate stub
# TODO: remove legacy github #261
if(CONFIG_USE_CONSOLE OR THECORE_CONFIG_USE_CONSOLE)
    msg_info("Console is enabled.")
    # Console is enabled and driver is provided by the platform.
    # Work here is to create aliases of bus/bus_pipe classes tied with console.
    target_link_libraries(core_cpp INTERFACE ${PLATFORM_NAME})

    # TODO: remove this definition, when #239 will be ready.
    # Also, related to the #260, since this definition should:
    # a) be provided by the platform
    # b) moved to the header rather than placed in CMake
    target_compile_definitions(core_cpp INTERFACE -DTHECORE_CONFIG_USE_CONSOLE=1)
    target_link_libraries(core_cpp INTERFACE bus)
else()
    msg_info("Console is not enabled.")
endif()

add_cppcheck(core_cpp UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)
target_link_libraries(core_cpp INTERFACE types)

add_unit_host_test(NAME istream_test
        SOURCES tests/istream_unit.cpp
        INC_DIRS export)

add_unit_host_test(NAME ostream_test
        SOURCES tests/ostream_unit.cpp
        INC_DIRS export)
