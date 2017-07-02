# Platform contains its own architectural definitions
set(THECORE_FORCE_NO_ARCH 1 CACHE "Disable architecture module" INTERNAL)

# Platform provides same stuff as sys
set(THECORE_FORCE_NO_SYS 1 CACHE "Disable sys module" INTERNAL)

# Platform depends on particle_firmware that, it turn, provides own newlib
set(THECORE_FORCE_NO_NEWLIB 1 CACHE "Disable newlib module" INTERNAL)

# Enabling FS causing build to fail:
#
# system_cloud.h:63:28: error: 'const CloudVariableTypeInt INT' redeclared as different kind of symbol
# const CloudVariableTypeInt INT;
#                          ^
# fat/pff/src/integer.h:24:16: note: previous declaration 'typedef int INT'
# typedef int    INT;
#
set(THECORE_FORCE_NO_FS_MODULE 1 CACHE "Disable fs module" INTERNAL)

# Device definitions
# TODO: drop it when side activities of #234 and #227 will be done
set(TARGET_MCU_IRQ_COUNT 1 CACHE STRING "Available interrupts")

#-------------------------------------------------------------------------------
# Generate serial driver definitions

set(IN_SERIAL_HDR ${CMAKE_CURRENT_LIST_DIR}/templates/serial_cfgs.in.hpp)
set(IN_SERIAL_CPP ${CMAKE_CURRENT_LIST_DIR}/templates/serial_dispatcher.in.cpp)
set(IN_DEFS_HDR ${CMAKE_CURRENT_LIST_DIR}/templates/defines.in.hpp)

set(OUT_SERIAL_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/aux/serial_cfgs.hpp)
set(OUT_SERIAL_CPP ${CMAKE_CURRENT_BINARY_DIR}/serial_dispatcher.cpp)
set(OUT_DEFS_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/aux/platform_defines.hpp)

if(DEFINED THECORE_CONFIG_PARTICLE_SERIALS_ENABLED)
    msg_info("Serial drivers enabled")
    # Definitions must come before input file
    list(APPEND COG_ARGS "-DSERIALS=\"${THECORE_CONFIG_PARTICLE_SERIALS_ENABLED}\"")
endif()

if(DEFINED THECORE_PARTICLE_CONSOLE_ENABLED)
    list(APPEND COG_ARGS "-DCONSOLE=${THECORE_PARTICLE_CONSOLE_ENABLED}")
endif()

theCore_create_cog_runner(IN ${IN_SERIAL_HDR} OUT ${OUT_SERIAL_HDR} ARGS ${COG_ARGS})
theCore_create_cog_runner(IN ${IN_SERIAL_CPP} OUT ${OUT_SERIAL_CPP} ARGS ${COG_ARGS})

#-------------------------------------------------------------------------------
# I2C configuration

set(I2C_ARGS)

if(THECORE_CONFIG_I2C_ENABLE)
    # Apply default configuration
    msg_info("I2C enabled")
    if(NOT DEFINED THECORE_CONFIG_I2C_SPEED)
        set(THECORE_CONFIG_I2C_SPEED 20000)
    endif()

    if(NOT DEFINED THECORE_CONFIG_I2C_STRETCH_CLK)
        set(THECORE_CONFIG_I2C_STRETCH_CLK 0)
    endif()

    set(I2C_ARGS
        "-DI2C_ENABLED=1;-DI2C_CFG=\"${THECORE_CONFIG_I2C_SPEED};${THECORE_CONFIG_I2C_STRETCH_CLK}\"")
endif()

set(IN_I2C_HDR ${CMAKE_CURRENT_LIST_DIR}/templates/i2c_wire_cfgs.in.hpp)
set(OUT_I2C_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/aux/i2c_wire_cfgs.hpp)

set(IN_I2C_CPP ${CMAKE_CURRENT_LIST_DIR}/templates/i2c_wire_dispatcher.in.cpp)
set(OUT_I2C_CPP ${CMAKE_CURRENT_BINARY_DIR}/i2c_wire_dispatcher.cpp)

theCore_create_cog_runner(IN ${IN_I2C_HDR} OUT ${OUT_I2C_HDR} ARGS ${I2C_ARGS})
theCore_create_cog_runner(IN ${IN_I2C_CPP} OUT ${OUT_I2C_CPP} ARGS ${I2C_ARGS})

#-------------------------------------------------------------------------------
# Systimer enable flag. Propagated by configure_file() below
if(THECORE_CONFIG_PARTICLE_USE_SYSTMR)
    set(USE_SYSTMR 1)
endif()

#-------------------------------------------------------------------------------
# Create a file with platform-related definitions
configure_file(${IN_DEFS_HDR} ${OUT_DEFS_HDR})

#-------------------------------------------------------------------------------
# Tie all auto-generated code together and specify correct dependencies

add_custom_target(platform_generated DEPENDS
    ${OUT_DEFS_HDR}
    ${OUT_SERIAL_CPP} ${OUT_SERIAL_HDR}
    ${OUT_I2C_CPP} ${OUT_I2C_HDR})

#-------------------------------------------------------------------------------

add_library(particle_electron
    ${CMAKE_CURRENT_LIST_DIR}/platform.cpp
    ${OUT_SERIAL_CPP} ${OUT_I2C_CPP})

add_library(particle_user_side INTERFACE)
target_sources(particle_user_side INTERFACE ${CMAKE_CURRENT_LIST_DIR}/user_side.cpp)

target_include_directories(particle_electron PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/export ${CMAKE_CURRENT_BINARY_DIR}/export)

target_link_libraries(particle_electron PUBLIC
    dbg particle_firmware utils bus)

target_link_libraries(particle_electron INTERFACE
    particle_user_side)

add_dependencies(particle_electron platform_generated)
