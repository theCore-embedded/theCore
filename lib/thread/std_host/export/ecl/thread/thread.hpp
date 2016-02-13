#ifndef LIB_THREAD_HOST_THREAD_HPP_
#define LIB_THREAD_HOST_THREAD_HPP_

#include <thread>
#include <ecl/err.hpp>
#include <ecl/assert.h>

namespace ecl
{

// TODO
class native_thread
{
public:
    native_thread();
    native_thread(int (*fn)(void *ctx), void *data);
    native_thread(native_thread &&other);
    ~native_thread();

    native_thread& operator=(native_thread &) = delete;

    ecl::err join();
private:
    std::thread m_thread;
    bool        m_default;   // Thread is in default state
};

}

#endif // LIB_THREAD_HOST_THREAD_HPP_
