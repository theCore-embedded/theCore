set(SRC_DIR ./freertos/FreeRTOS/Source/)

## Qt creator hack
file(GLOB_RECURSE ALL_HEADERS "${SRC_DIR}include/*.h")
add_custom_target(free_rtos_dummy_target SOURCES "${ALL_HEADERS}")
## EO hack

add_library(freertos 
	${SRC_DIR}croutine.c
	${SRC_DIR}event_groups.c
	${SRC_DIR}list.c
	${SRC_DIR}queue.c
	${SRC_DIR}tasks.c
	${SRC_DIR}timers.c
	${SRC_DIR}portable/GCC/ARM_CM4F/port.c
	${SRC_DIR}portable/GCC/ARM_CM4F/portmacro.h
	${SRC_DIR}portable/MemMang/heap_1.c
	)

target_include_directories(freertos PUBLIC
	.
	${SRC_DIR}include/
	${SRC_DIR}portable/GCC/ARM_CM4F/)

target_link_libraries(freertos sys)
target_link_libraries(freertos target)
