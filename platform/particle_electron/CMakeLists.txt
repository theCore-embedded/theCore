add_library(particle_electron INTERFACE)
target_sources(particle_electron INTERFACE ${CMAKE_CURRENT_LIST_DIR}/platform.cpp)
target_include_directories(particle_electron INTERFACE export)

target_link_libraries(particle_electron INTERFACE
    dbg particle_firmware utils bus)

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

set(OUT_SERIAL_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/platform/serial_cfgs.hpp)
set(OUT_SERIAL_CPP ${CMAKE_CURRENT_BINARY_DIR}/serial_dispatcher.cpp)
set(OUT_DEFS_HDR ${CMAKE_CURRENT_BINARY_DIR}/export/platform/defines.hpp)

if(DEFINED THECORE_PARTICLE_SERIALS_ENABLED)
    # Definitions must come before input file
    list(APPEND COG_ARGS "-DSERIALS=\"${THECORE_PARTICLE_SERIALS_ENABLED}\"")
endif()

if(DEFINED THECORE_PARTICLE_CONSOLE_ENABLED)
    list(APPEND COG_ARGS "-DCONSOLE=${THECORE_PARTICLE_CONSOLE_ENABLED}")
endif()

list(APPEND COG_HDR_ARGS ${COG_ARGS} -d -o ${OUT_SERIAL_HDR} ${IN_SERIAL_HDR})
list(APPEND COG_CPP_ARGS ${COG_ARGS} -d -o ${OUT_SERIAL_CPP} ${IN_SERIAL_CPP})

add_custom_command(
    OUTPUT ${OUT_SERIAL_CPP} ${OUT_SERIAL_HDR}
    COMMAND cog.py ${COG_HDR_ARGS}
    COMMAND cog.py ${COG_CPP_ARGS}
    DEPENDS ${IN_SERIAL_HDR} ${IN_SERIAL_CPP}
    COMMENT "Generating ${OUT_SERIAL_HDR} and ${OUT_SERIAL_CPP}"
)

# Write a dummy file to avoid errors with INTERFACE libraries
# and not-yet-generated source files.
if(NOT EXISTS ${OUT_SERIAL_CPP})
    file(WRITE ${OUT_SERIAL_CPP} "")
endif()

#-------------------------------------------------------------------------------
# Systimer enable flag. Propagated by configure_file() below
if(THECORE_PARTICLE_USE_SYSTMR)
    message("Hello!**********")
    set(USE_SYSTMR 1)
endif()

# Create a file with platform-related definitions
configure_file(${IN_DEFS_HDR} ${OUT_DEFS_HDR})

#-------------------------------------------------------------------------------

# Tie all auto-generated code together and specify correct dependencies

add_custom_target(platform_generated DEPENDS ${OUT_SERIAL_HDR} ${OUT_DEFS_HDR} ${OUT_SERIAL_CPP})
add_dependencies(particle_electron platform_generated)
target_include_directories(particle_electron INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/export/)
target_sources(particle_electron INTERFACE ${OUT_SERIAL_CPP})
