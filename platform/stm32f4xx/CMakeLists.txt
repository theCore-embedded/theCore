add_library(stm32f4xx STATIC
		platform.cpp
		irq_manager.cpp
		exti_manager.cpp
		utils.c)

target_include_directories(stm32f4xx PUBLIC export)

# Platform submodules
add_subdirectory(startup)
add_subdirectory(SPL)
add_subdirectory(CMSIS)

# TODO: clarify essential libraries
target_link_libraries(stm32f4xx PUBLIC spl)
target_link_libraries(stm32f4xx PUBLIC common_spi)
target_link_libraries(stm32f4xx PUBLIC common_pin)
target_link_libraries(stm32f4xx PUBLIC common_usart)
target_link_libraries(stm32f4xx PUBLIC common_i2c)
target_link_libraries(stm32f4xx PUBLIC common_bus)
target_link_libraries(stm32f4xx PUBLIC types)
target_link_libraries(stm32f4xx PUBLIC containers)
target_link_libraries(stm32f4xx PUBLIC emc) # For memset

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

message(STATUS "Checking [CONFIG_PLATFORM_DEVICE]...")
if (CONFIG_PLATFORM_DEVICE STREQUAL STM32F40_41xxx)
	# Count of user-overridable interrupts.
	# For this particular platform it doesn't include system exceptions,
	# but only peripheral interrupts. Yet.
	target_compile_definitions(stm32f4xx PUBLIC -DCONFIG_IRQ_COUNT=82)
else()
	# Additional implementation will be required to cover unsupported
	# devices from stm32f4xx line.
	message(FATAL_ERROR "Not supported device specified: ${CONFIG_PLATFORM_DEVICE}")
endif()

target_compile_definitions(stm32f4xx PUBLIC
	-DCONFIG_MAX_ISR_PRIORITY=${CONFIG_MAX_ISR_PRIORITY})

# Pick console driver
message(STATUS "STM32F4XX: Checking [CONFIG_USE_CONSOLE]...")
if(${CONFIG_USE_CONSOLE})
	if(NOT DEFINED CONFIG_CONSOLE_DEVICE_NUM)
		message(FATAL_ERROR "CONFIG_CONSOLE_DEVICE_NUM must be set in order to use console")
	endif()

	if(NOT DEFINED CONFIG_CONSOLE_BAUD)
		message(FATAL_ERROR "CONFIG_CONSOLE_BAUD must be set in order to use console")
	endif()

	configure_file(templates/platform_console.hpp.in
			${CMAKE_CURRENT_BINARY_DIR}/console/platform_console.hpp)
	target_include_directories(stm32f4xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/console/)

	# Helps to determine if bypass driver must be enabled and inited
	target_compile_definitions(stm32f4xx PRIVATE -DCONFIG_BYPASS_CONSOLE_ENABLED)
	# Bypass console implementation
	target_sources(stm32f4xx PRIVATE bypass_console.cpp)
	message(STATUS "Bypass console is supported and enabled by the platform")
else()
	configure_file(templates/platform_console_stub.hpp.in
			${CMAKE_CURRENT_BINARY_DIR}/console/platform_console.hpp)
	target_include_directories(stm32f4xx PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/console/)
endif()
