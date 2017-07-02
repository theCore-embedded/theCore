add_library(freertos_main kernel_main.c)
target_link_libraries(freertos_main PRIVATE dbg)

set(MAIN_STACK_SIZE)
if(DEFINED CONFIG_FREERTOS_MAIN_THREAD_STACK)
	set(MAIN_STACK_SIZE ${CONFIG_FREERTOS_MAIN_THREAD_STACK})
else()
	set(MAIN_STACK_SIZE 512)
endif()

msg_info("FreeRTOS main thread stack size: ${MAIN_STACK_SIZE} bytes")

target_compile_definitions(freertos_main PRIVATE -DMAIN_STACK_SIZE=${MAIN_STACK_SIZE})

if(CONFIG_OS_EXTERNAL)
	msg_trace("Kernel is selected, but it is provided externally")
	return()
endif()

set(SRC_DIR freertos/FreeRTOS/Source/)

# Compiler-dependent paths.
# If you hit error here, make sure you check compiler support
# in FreeRTOS source tree. If compiler is supported by RTOS, but not
# by build system just add another option here.
if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER MATCHES "gcc")
	set(PORT_DIR "GCC/")
else()
	msg_fatal("Compiler is not supported by FreeRTOS build: "
		"${CMAKE_C_COMPILER_ID}")
endif()

# Path to FreeRTOS header must be provided
if(NOT DEFINED CONFIG_FREERTOS_HEADERS_PATH)
    msg_fatal("FreeRTOS header path is not set")
else()
    if(NOT EXISTS ${CONFIG_FREERTOS_HEADERS_PATH}/FreeRTOSConfig.h)
        msg_fatal("Path \'${CONFIG_FREERTOS_HEADERS_PATH}\' does not contain FreeRTOSConfig.h")
    endif()
endif()

# Architecture-dependent paths.
# Same as for compiler paths works here. If the architecture is supported
# by FreeRTOS but not by this build system then add another option here.
if(TARGET_MCU_ARCH MATCHES "arm_cm4")
	set(PORT_DIR ${PORT_DIR}ARM_CM4F/)
elseif(TARGET_MCU_ARCH MATCHES "arm_cm3")
	set(PORT_DIR ${PORT_DIR}ARM_CM3/)
else()
	msg_fatal("Processor arch is not supported by FreeRTOS build: "
		"${TARGET_MCU_ARCH}")
endif()

add_library(freertos
	${SRC_DIR}croutine.c
	${SRC_DIR}event_groups.c
	${SRC_DIR}list.c
	${SRC_DIR}queue.c
	${SRC_DIR}tasks.c
	${SRC_DIR}timers.c
	${SRC_DIR}portable/${PORT_DIR}port.c
	${SRC_DIR}portable/MemMang/heap_1.c # TODO: make it configurable as well
	)

# Depend on locally built FreeRTOS
target_link_libraries(freertos_main PRIVATE freertos)

# TODO: better check for CONFIG_FREERTOS_HEADERS_PATH
target_include_directories(freertos PUBLIC
	${SRC_DIR}include/
	${CONFIG_FREERTOS_HEADERS_PATH}/
	${SRC_DIR}portable/${PORT_DIR}/
)
