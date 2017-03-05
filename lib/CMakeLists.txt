if(CMAKE_CROSSCOMPILING)
    # Usable only for bare-metal environment.
    # TODO: not all platfotms that theCore will support are bare-metals.
    # Some platforms may not need newlib. This should be checked.
    add_subdirectory(newlib)
endif()

add_subdirectory(cpp)
add_subdirectory(allocators)
add_subdirectory(utils)
add_subdirectory(debug)
add_subdirectory(types)
add_subdirectory(thread)
add_subdirectory(containers)
add_subdirectory(cmsis)
add_subdirectory(stm32-spl)
add_subdirectory(ti-tivaware)

# Extra libraries, not enabled by default
add_library(core_extra_libs INTERFACE)

# Not enabled by default. Enabling this requires cloning of FAT submodule.
if(CONFIG_USE_FS OR BUILD_TESTING)
    add_subdirectory(fs)
endif()

# Not enabled by default. Enabling this requires cloning of Unity submodule.
if(CONFIG_USE_UNITY OR BUILD_TESTING)
    add_subdirectory(unity)
    target_link_libraries(core_extra_libs INTERFACE unity)
endif()
