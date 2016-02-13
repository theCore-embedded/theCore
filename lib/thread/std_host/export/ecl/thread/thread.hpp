#ifndef LIB_THREAD_HOST_THREAD_HPP_
#define LIB_THREAD_HOST_THREAD_HPP_

#include <thread>
#include <ecl/err.hpp>
#include <ecl/assert.h>

namespace ecl
{

// TODO
class exposed_thread
{
public:
    exposed_thread();
    exposed_thread(void (*fn)(void *ctx), void *data);
    exposed_thread(exposed_thread &&other);
    ~exposed_thread();

    exposed_thread& operator=(exposed_thread &) = delete;

    ecl::err join();
private:
    std::thread m_thread;
    bool        m_default;   // Thread is in default state
};

}

#endif // LIB_THREAD_HOST_THREAD_HPP_
