#ifndef LIB_THREAD_MUTEX_
#define LIB_THREAD_MUTEX_

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
};


}

#endif // LIB_THREAD_MUTEX_
