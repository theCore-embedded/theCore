#ifndef KERNEL_OS_FREERTOS
#define KERNEL_OS_FREERTOS


void* semaphore_create();
void semaphore_take(void *sem);
void semaphore_give(void *sem);
void delay(unsigned int msecs);
int task_create(void(*routine)(void *), const char *name, int stack_sz);
void schedule_start();

#endif
