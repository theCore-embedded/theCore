#ifndef LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
#define LIB_THREAD_FREERTOS_SEMAPHORE_HPP_

#include <FreeRTOS.h>
#include <semphr.h>

namespace ecl
{

class semaphore
{
public:
    semaphore();
    ~semaphore();

    void signal();
    void wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    SemaphoreHandle_t m_semaphore;
};

}

#endif // LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
