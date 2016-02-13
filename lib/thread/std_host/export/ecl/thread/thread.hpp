#ifndef LIB_THREAD_HOST_THREAD_HPP_
#define LIB_THREAD_HOST_THREAD_HPP_

#include <thread>

namespace ecl
{

// TODO
class exposed_thread
{
public:
    exposed_thread(void (*fn)(void *ctx), void *data);
    exposed_thread(exposed_thread &&other);
    ~exposed_thread();

    exposed_thread& operator=(exposed_thread &) = delete;

    void join();
private:
    std::thread m_thread;
};

}

#endif // LIB_THREAD_HOST_THREAD_HPP_
