#ifndef LIB_THREAD_FREERTOS_MUTEX_HPP_
#define LIB_THREAD_FREERTOS_MUTEX_HPP_

#include <FreeRTOS.h>
#include <semphr.h>

namespace ecl
{

class mutex
{
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();
    bool try_lock();

    mutex(const mutex&)             = delete;
    mutex& operator=(const mutex&)  = delete;
private:
    SemaphoreHandle_t m_mutex;
};

}

#endif // LIB_THREAD_FREERTOS_MUTEX_HPP_
