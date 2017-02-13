//! \file
//! \brief Mutex implementation for projects without OS support.

#ifndef LIB_THREAD_NO_OS_MUTEX_
#define LIB_THREAD_NO_OS_MUTEX_

namespace ecl
{

class mutex
{
public:
    mutex() = default;

    void lock();
    void unlock();
    bool try_lock();

    mutex(const mutex&)             = delete;
    mutex& operator=(const mutex&)  = delete;
};

} // namespace ecl

#endif // LIB_THREAD_NO_OS_MUTEX_
