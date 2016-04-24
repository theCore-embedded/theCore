add_library(stm32f4xx STATIC platform.cpp irq_manager.cpp)
add_library(stm32f4xx_utils STATIC utils.c)

target_include_directories(stm32f4xx PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)
add_subdirectory(CMSIS)

# TODO: clarify essential libraries
target_link_libraries(stm32f4xx spl)
target_link_libraries(stm32f4xx common_spi)
target_link_libraries(stm32f4xx common_pin)
target_link_libraries(stm32f4xx common_usart)
target_link_libraries(stm32f4xx common_bus)
target_link_libraries(stm32f4xx types)

add_cppcheck(stm32f4xx)

# Notify whole system about which processor is used
set(TARGET_PROCESSOR_ARCHITECTURE "arm_cm4" CACHE STRING "Processor arch")

# Maximum priority of user ISRs can be redefined,
# but be careful: it heavily depends on RTOS constraits.
# I.e. for FreeRTOS maximum user priorities should be logically
# less or equal than configMAX_SYSCALL_INTERRUPT_PRIORITY
# (numerical priority representation of M4 is such that higher numbers
# corresponds to smaller logical priorities)

message(STATUS "Checking [CONFIG_MAX_ISR_PRIORITY]...")
if(NOT DEFINED CONFIG_MAX_ISR_PRIORITY)
	set(CONFIG_MAX_ISR_PRIORITY 0xff)
	message(STATUS "CONFIG_MAX_ISR_PRIORITY not set,"
		" using default value: ${CONFIG_MAX_ISR_PRIORITY}")
endif()

target_compile_definitions(
	stm32f4xx
	PUBLIC
	-DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY})


