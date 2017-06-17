if(__platform_api_def)
    return()
endif()
set(__platform_api_def YES)

#-------------------------------------------------------------------------------
# Particle Electron private build API. Not intended to be called by the user.

# Enables particular serial.
function(__theCore_enable_serial NUM)
    if (NOT DEFINED THECORE_CONFIG_PARTICLE_SERIALS_ENABLED)
        set(THECORE_CONFIG_PARTICLE_SERIALS_ENABLED "${NUM}" CACHE INTERNAL "Serial device flag")
    else()
        set(TMP_LIST ${THECORE_CONFIG_PARTICLE_SERIALS_ENABLED})
        list(APPEND TMP_LIST ${NUM})
        list(REMOVE_DUPLICATES TMP_LIST)

        set(THECORE_CONFIG_PARTICLE_SERIALS_ENABLED "${TMP_LIST}" CACHE INTERNAL "Serial device flag")
    endif()
endfunction()

#-------------------------------------------------------------------------------
# Particle Electron public build API.

# Enables Serial console, exposed over USB.
function(theCore_electron_enable_console)
    # Enable console automatically on Serial0
    # TODO: move out it to the main platform CMake file
    set(THECORE_PARTICLE_CONSOLE_ENABLED "0" CACHE INTERNAL "Serial bypass")
    set(THECORE_CONFIG_USE_CONSOLE "YES" CACHE BOOL "Console is enabled")
    __theCore_enable_serial(0)
endfunction()

# Enables Serial1.
function(theCore_electron_enable_serial1)
    __theCore_enable_serial(1)
endfunction()

# Enables Serial2.
function(theCore_electron_enable_serial2)
    __theCore_enable_serial(2)
endfunction()

# Enables Serial4.
function(theCore_electron_enable_serial4)
    __theCore_enable_serial(4)
endfunction()

# Enables Serial5.
function(theCore_electron_enable_serial5)
    __theCore_enable_serial(5)
endfunction()

# Allows theCore to use Particle Electron timers
function(theCore_electron_use_systmr)
    set(THECORE_CONFIG_PARTICLE_USE_SYSTMR 1 CACHE BOOL "Allows to use timer by theCore" FORCE)
endfunction()

# Enables I2C interface in the Particle Electron platform.
# Syntax:
# theCore_electron_enable_i2c([SPEED speed] [STRETCH_CLK])
#   - speed - I2C bus speed.
#   - STRETCH_CLK - If present, enables I2C clock stretching.
function(theCore_electron_enable_i2c)
    cmake_parse_arguments(
            I2C_CFG
            "STRETCH_CLK"
            "SPEED"
            ""
            ${ARGN}
    )

    set(THECORE_CONFIG_I2C_ENABLE 1 CACHE BOOL "I2C is enabled" FORCE)

    if(I2C_CFG_STRETCH_CLK)
        set(THECORE_CONFIG_I2C_STRETCH_CLK 1 CACHE BOOL "I2C clock stretch option" FORCE)
    endif()

    if(DEFINED I2C_CFG_SPEED)
        set(THECORE_CONFIG_I2C_SPEED "${I2C_CFG_SPEED}" CACHE STRING "I2C speed" FORCE)
    endif()
endfunction()