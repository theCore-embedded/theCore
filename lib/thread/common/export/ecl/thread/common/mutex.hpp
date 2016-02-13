#ifndef LIB_THREAD_COMMON_MUTEX_
#define LIB_THREAD_COMMON_MUTEX_

namespace ecl
{

namespace common
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
};

}

}

#endif // LIB_THREAD_COMMON_MUTEX_
