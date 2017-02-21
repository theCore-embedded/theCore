# Check if separate manager is really required.
# NVIC or similar will do it by itself.
# Host platform is not in need of external IRQ manager as well.
if(PLATFORM_NAME STREQUAL host OR TARGET_USE_NVIC_IRQ_NAMES)
    add_library(platform_common INTERFACE)
    target_include_directories(platform_common INTERFACE export)
else()
    # IRQ management must be enabled only if cross-compiling for embedded boards

    add_library(platform_common irq.cpp)

    target_include_directories(platform_common PUBLIC export)

    target_link_libraries(platform_common PUBLIC ${PLATFORM_NAME} types)
    target_link_libraries(platform_common PRIVATE dbg)

    target_compile_definitions(platform_common PRIVATE -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})
endif()

add_unit_host_test(NAME mmio
        SOURCES tests/mmio_unit.cpp
        INC_DIRS export)



