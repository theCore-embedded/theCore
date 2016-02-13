#ifndef LIB_THREAD_FREERTOS_THREAD_HPP_
#define LIB_THREAD_FREERTOS_THREAD_HPP_

#include <FreeRTOS.h>
#include <task.h>
#include <ecl/err.hpp>

namespace ecl
{

// TODO
class native_thread
{
public:
    native_thread();
    native_thread(int (*fn)(void *), void *data);
    native_thread(native_thread &&other);
    ~native_thread();

    native_thread& operator=(native_thread &) = delete;

    ecl::err join();
private:
    TaskHandle_t m_task;
};


#endif // LIB_THREAD_FREERTOS_THREAD_HPP_
