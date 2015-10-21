set(SRC_DIR ./freertos/FreeRTOS/Source/)
add_library(freertos 
	./freertos/FreeRTOS/Source/croutine.c
	./freertos/FreeRTOS/Source/event_groups.c
	./freertos/FreeRTOS/Source/list.c
	./freertos/FreeRTOS/Source/queue.c
	./freertos/FreeRTOS/Source/tasks.c
	./freertos/FreeRTOS/Source/timers.c
	./freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
	./freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F/portmacro.h
	)

target_include_directories(freertos PUBLIC
	.
	./freertos/FreeRTOS/Source/include/
	./freertos/FreeRTOS/Source/portable/GCC/ARM_CM4F/)
