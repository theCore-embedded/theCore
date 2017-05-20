if(CMAKE_CROSSCOMPILING)
    # Usable only for bare-metal environment.
    # TODO: not all platfotms that theCore will support are bare-metals.
    # Some platforms may not need newlib. This should be checked.
    if(TARGET_FORCE_NO_NEWLIB)
        message(STATUS "Newlib is disabled")
    else()
        add_subdirectory(newlib)
    endif()
endif()

add_subdirectory(cpp)
add_subdirectory(allocators)
add_subdirectory(fs)
add_subdirectory(utils)
add_subdirectory(debug)
add_subdirectory(types)
add_subdirectory(thread)
add_subdirectory(containers)
add_subdirectory(cmsis)
add_subdirectory(stm32-spl)
add_subdirectory(ti-tivaware)
add_subdirectory(unity)
add_subdirectory(particle_firmware)
