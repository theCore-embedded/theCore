#ifndef LIB_THREAD_EMPTY_SEMAPHORE_
#define LIB_THREAD_EMPTY_SEMAPHORE_

#include "spinlock.hpp"

namespace ecl
{

class semaphore
{
public:
    constexpr semaphore();

    void signal();
    void wait();

private:
    spinlock m_lock;
};

}

#endif // LIB_THREAD_EMPTY_SEMAPHORE_
