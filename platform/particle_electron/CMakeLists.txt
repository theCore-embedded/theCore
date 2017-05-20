add_library(particle_electron INTERFACE)
target_sources(particle_electron INTERFACE ${CMAKE_CURRENT_LIST_DIR}/platform.cpp)
target_include_directories(particle_electron INTERFACE export)

target_link_libraries(particle_electron INTERFACE
    dbg particle_firmware utils bus)

# Platform contains its own architectural definitions
set(TARGET_FORCE_NO_ARCH 1 CACHE BOOL "Disable architecture module" FORCE)

# Platform provides same stuff as sys
set(TARGET_FORCE_NO_SYS 1 CACHE BOOL "Disable sys module" FORCE)

# Platform depends on particle_firmware that, it turn, provides own newlib
set(TARGET_FORCE_NO_NEWLIB 1 CACHE BOOL "Disable newlib module" FORCE)


# Device definitions
# TODO: use real count
set(TARGET_MCU_IRQ_COUNT 1 CACHE STRING "Available interrupts")

#-------------------------------------------------------------------------------
# Generate serial driver definitions

set(IN_SERIAL_HDR ${CMAKE_CURRENT_LIST_DIR}/templates/serial_cfgs.in.hpp)
set(IN_SERIAL_CPP ${CMAKE_CURRENT_LIST_DIR}/templates/serial_cfgs.in.cpp)

set(OUT_SERIAL_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/platform/serial_cfgs.hpp)

if(DEFINED THECORE_PARTICLE_SERIALS_ENABLED)
    # Definitions must come before input file
    list(APPEND COG_ARGS "-DSERIALS=\"${THECORE_PARTICLE_SERIALS_ENABLED}\"")
endif()

if(DEFINED THECORE_PARTICLE_CONSOLE_ENABLED)
    list(APPEND COG_ARGS "-DCONSOLE=${THECORE_PARTICLE_CONSOLE_ENABLED}")
endif()

list(APPEND COG_ARGS -d -o ${OUT_SERIAL_HDR} ${IN_SERIAL_HDR})

add_custom_command(
    OUTPUT ${OUT_SERIAL_HDR}
    COMMAND cog.py ${COG_ARGS}
    DEPENDS ${IN_SERIAL_HDR}
    COMMENT "Generating ${OUT_SERIAL_HDR}."
)

#-------------------------------------------------------------------------------

# Tie all auto-generated code together and specify correct dependenices

add_custom_target(platform_generated DEPENDS ${OUT_SERIAL_HDR})
add_dependencies(particle_electron platform_generated)
target_include_directories(particle_electron INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/export/)
