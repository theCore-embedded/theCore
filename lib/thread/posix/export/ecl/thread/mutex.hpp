#ifndef LIB_THREAD_HOST_MUTEX_
#define LIB_THREAD_HOST_MUTEX_

#include <mutex>

namespace ecl
{

class mutex
{
public:
    constexpr mutex();

    void lock();
    void unlock();
    bool try_lock();

    mutex(const mutex&)             = delete;
    mutex& operator=(const mutex&)  = delete;
private:
    std::mutex m_mutex;
};

}


#endif // LIB_THREAD_HOST_MUTEX_
