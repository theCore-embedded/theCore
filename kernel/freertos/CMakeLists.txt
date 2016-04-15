add_library(freertos_main kernel_main.c)
#target_link_libraries(freertos_main PRIVATE main)
target_link_libraries(freertos_main PRIVATE utils)

set(MAIN_STACK_SIZE)
if (DEFINED CONFIG_FREERTOS_MAIN_THREAD_STACK)
	set(MAIN_STACK_SIZE ${CONFIG_FREERTOS_MAIN_THREAD_STACK})
else()
	set(MAIN_STACK_SIZE 512)
endif()

message(STATUS "FreeRTOS main thread stack size: ${MAIN_STACK_SIZE} bytes")

target_compile_definitions(freertos_main PRIVATE -DMAIN_STACK_SIZE=${MAIN_STACK_SIZE})

if (NOT ${CONFIG_OS_INTERNAL})
	message(STATUS "Kernel is selected, but it is provdided externally")
	return()
endif()

set(SRC_DIR freertos/FreeRTOS/Source/)

# Obtain some processor info
set(TARGET_ARCH ${TARGET_PROCESSOR_ARCHITECTURE})

# Compiler-dependent paths.
# If you hit error here, make sure you check compiler support
# in FreeRTOS source tree. If compiler is supported by RTOS, but not
# by build system just add another option here.
if (${CMAKE_C_COMPILER_ID} MATCHES GNU)
	set(PORT_DIR "GCC/")
else()
	message(FATAL_ERROR "Compiler is not supported by FreeRTOS build: "
		"${CMAKE_C_COMPILER_ID}")
endif()

# Architecture-dependent paths.
# Same as for compiler paths works here. If the architecture is supported
# by FreeRTOS but not by this build system then add another option here.
if (${TARGET_ARCH} MATCHES arm_cm4)
	set(PORT_DIR ${PORT_DIR}ARM_CM4F/)
else()
	message(FATAL_ERROR "Processor arch is not supported by FreeRTOS build: "
		"${TARGET_ARCH}")
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
