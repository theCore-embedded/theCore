add_library(platform_common irq.cpp)

target_link_libraries(platform_common PUBLIC ${PLATFORM_NAME})
target_link_libraries(platform_common PRIVATE dbg)

target_include_directories(platform_common PUBLIC export)
target_compile_definitions(platform_common PRIVATE -DIRQ_COUNT=${TARGET_MCU_IRQ_COUNT})
