# Linker flags. TODO: check redundancy when using -msoft-float and -mfloat-abi=soft together
set(CMAKE_C_LINK_FLAGS
        "--specs=nano.specs -fno-strict-aliasing -ffunction-sections \
        -fdata-sections -mcpu=cortex-m3 -mthumb -nostartfiles -fno-builtin \
        -nostartfiles -Xlinker --gc-sections"
    CACHE STRING "Linker C flags" FORCE)
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} CACHE STRING "Linker C++ flags" FORCE)
