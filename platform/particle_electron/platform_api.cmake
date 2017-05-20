if(__platform_api_def)
    return()
endif()
set(__platform_api_def YES)

#-------------------------------------------------------------------------------
# Particle Electron private build API. Not intented to be called by the user.

# Enables particular serial.
function(__theCore_enable_serial NUM)
    if (NOT DEFINED THECORE_PARTICLE_SERIALS_ENABLED)
        set(THECORE_PARTICLE_SERIALS_ENABLED "${NUM}" CACHE INTERNAL "Serial device flag")
    else()
        set(TMP_LIST ${THECORE_PARTICLE_SERIALS_ENABLED})
        list(APPEND TMP_LIST ${NUM})
        list(REMOVE_DUPLICATES TMP_LIST)

        set(THECORE_PARTICLE_SERIALS_ENABLED "${TMP_LIST}" CACHE INTERNAL "Serial device flag")
    endif()
endfunction()

#-------------------------------------------------------------------------------
# Particle Electron public build API.

# Enables Serial console, exposed over USB.
function(theCore_electron_enable_console)
    # Enable console automatically on Serial0
    set(THECORE_PARTICLE_CONSOLE_ENABLED "0" CACHE INTERNAL "Serial bypass")
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
