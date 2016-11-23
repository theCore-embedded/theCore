if("${PLATFORM_NAME}" STREQUAL "host")
    add_library(platform_common INTERFACE)
    target_include_directories(platform_common INTERFACE export)
else()
    # IRQ management must be enabled only if cross-compiling for embedded boards

    add_library(platform_common irq.cpp)

    target_include_directories(platform_common PUBLIC export)

    target_link_libraries(platform_common PUBLIC ${PLATFORM_NAME})
    target_link_libraries(platform_common PRIVATE dbg)

    target_compile_definitions(platform_common PRIVATE -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})
endif()


