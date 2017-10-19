# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

if(CMAKE_CROSSCOMPILING)
    # Usable only for bare-metal environment.
    # TODO: not all platfotms that theCore will support are bare-metals.
    # Some platforms may not need newlib. This should be checked.
    if(THECORE_FORCE_NO_NEWLIB)
        msg_info("Newlib is disabled")
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
